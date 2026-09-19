#!/usr/bin/env python3
"""
Adaptive chunked builder for !RDPClient on the RISC OS build service
(build.riscos.online).

WHY THIS EXISTS
    The full app is ~71 heavy C compiles plus a link. Each cc invocation on the
    emulated service costs ~30-60s (huge OpenSSL/rdesktop header chains), so a
    single-shot build blows past the service's ~500-650s wall-clock cap. This
    driver splits the compiles into slices that each finish under the cap,
    carrying the accumulated object files (the 'o' directory) forward between
    builds -- exactly the pattern that built DeskLib. It then runs one final
    build that creates the tabd/pane2 libraries and links everything.

    It is ADAPTIVE: it does not need to know the per-file timing. If a slice
    hits the wall cap (RC 124 / no completion), it halves the slice and retries
    from the same position. Real compile errors (throwbacks) stop the run and
    are reported so you can see exactly what failed.

USAGE
    python3 -m pip install websocket-client      # once
    python3 buildapp.py                          # from the folder holding
                                                 # buildapp.py, plan.json,
                                                 # app_base.zip

    Resuming: object files accumulate in ./work/o, so re-running continues
    where it left off (already-built objects are shipped in the zip and not
    recompiled). Delete ./work to start clean.

OUTPUT
    RDPClient        the linked absolute (RISC OS filetype &FF8), ready to drop
                     into !RDPClient. Also a per-step log: buildapp.log
"""
import sys, os, re, json, base64, time, zipfile, shutil, io, subprocess, tempfile, struct

try:
    import websocket  # 'websocket-client'
except ImportError:
    sys.exit("Missing dependency. Run:  python3 -m pip install websocket-client")

HERE      = os.path.dirname(os.path.abspath(__file__))
PLAN      = os.path.join(HERE, "plan.json")
BASE_ZIP  = os.path.join(HERE, "app_base.zip")
WORK      = os.path.join(HERE, "work")
SERVER    = "wss://build.riscos.online/ws"
ARCH      = "aarch32"

# Per-chunk wall budget we ASK the service for. The service enforces its own
# cap (~500-650s); we request a bit under and rely on adaptive shrink anyway.
CHUNK_TIMEOUT = 480
START_SLICE   = 8      # initial compiles per chunk; shrinks on wall-cap hits
MIN_SLICE     = 1

LOG = open(os.path.join(HERE, "buildapp.log"), "w", encoding="utf-8", errors="replace")
def log(s=""):
    print(s, flush=True)
    try: LOG.write(s + "\n"); LOG.flush()
    except Exception: pass


def load_plan():
    p = json.load(open(PLAN))
    return p["setup"], p["compiles"], p["finals"]


def fresh_work():
    # Always sync the latest sources from app_base.zip into work/, so a
    # redelivered app_base.zip (with fixed sources) actually takes effect on the
    # next run. Carried objects live in work/o, which app_base.zip does not
    # contain, so extracting over the tree preserves them. A version stamp keeps
    # the log honest about whether anything changed.
    os.makedirs(WORK, exist_ok=True)
    try:
        stamp = str(os.path.getmtime(BASE_ZIP)) + ":" + str(os.path.getsize(BASE_ZIP))
    except OSError:
        stamp = "?"
    spath = os.path.join(WORK, ".base_stamp")
    prev = open(spath).read().strip() if os.path.exists(spath) else None
    if prev == stamp:
        log("Sources in work/ already match app_base.zip.")
    else:
        log("Syncing sources from app_base.zip -> work/ (%s)"
            % ("first unpack" if prev is None else "app_base.zip changed"))
        with zipfile.ZipFile(BASE_ZIP) as z:
            z.extractall(WORK)          # overwrites source files; leaves work/o/*.o
        open(spath, "w").write(stamp)
    os.makedirs(os.path.join(WORK, "o"), exist_ok=True)
    # If a locally-rebuilt 32-bit DeskLib is present (produced by
    # builddesklib.py as ./DeskLib32), use it in place of the one shipped in
    # app_base.zip. Lets the DeskLib rebuild and the app build stay decoupled.
    dl32 = os.path.join(HERE, "DeskLib32")
    if os.path.isdir(dl32):
        # Case-insensitive FS: the bare name resolves to the desklib32/ repo
        # dir; the actual prebuilt library file lives inside it.
        dl32 = os.path.join(dl32, "DeskLib32")
    if os.path.isfile(dl32):
        dst = os.path.join(WORK, "desklib", "o", "DeskLib")
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.copyfile(dl32, dst)
        log("Using local 32-bit DeskLib32 (%d bytes) in place of app_base's."
            % os.path.getsize(dl32))


def built_objs():
    """Set of object basenames already present in work/o (e.g. 'o.RDPClient')."""
    od = os.path.join(WORK, "o")
    out = set()
    for n in os.listdir(od):
        if n == ".keep":
            continue
        out.add("o." + n)
    return out


# --- chunk-5 guard -------------------------------------------------------
# The C99 printf-family (snprintf/vsnprintf) lives in a SharedCLibrary stub
# chunk (chunk 5) that the Pi's ROM shared C library (5.34) cannot initialise:
# linking an object that imports one produces a garbage LibInit at launch and
# the "SWI &5DC34" crash. Our code must format via a bounded vsprintf wrapper
# (cm_snprintf / ce_snprintf / kh_snprintf) instead. This guard scans the
# locally-built objects that go into the link and FAILS the build if any imports
# such a symbol -- as a whole symbol, so the wrapper names (which merely contain
# "snprintf" as a substring, e.g. kh_snprintf) do not trip it.
_CHUNK5_SYMS = (b"snprintf", b"vsnprintf")

def _obj_imports_c99(data):
    hits = set()
    for sym in _CHUNK5_SYMS:
        for m in re.finditer(re.escape(sym) + b"\x00", data):
            i = m.start()
            prev = data[i - 1] if i > 0 else 0
            if not (chr(prev).isalnum() or prev == 0x5f):   # 0x5f = '_'
                hits.add(sym.decode())
    return sorted(hits)

def chunk5_guard():
    """List of (object_name, [symbols]) for local link-set objects that import a
    chunk-5 C99 symbol. Empty == clean."""
    odir = os.path.join(WORK, "o")
    linkvia = os.path.join(WORK, "linkvia")
    names = []
    if os.path.isfile(linkvia):
        for ln in open(linkvia):
            ln = ln.strip()
            # local objects look like "o.Name"; skip C:/DeskLib:/TCPIPLibs:/OpenSSLLib: libs
            if ln.startswith("o.") and ":" not in ln:
                names.append(ln[2:])
    if not names:                       # fallback: scan every real object present
        names = [n for n in os.listdir(odir)
                 if n != ".keep" and ".stale" not in n and ".bak" not in n]
    bad = []
    for n in names:
        fp = os.path.join(odir, n)
        if os.path.isfile(fp):
            syms = _obj_imports_c99(open(fp, "rb").read())
            if syms:
                bad.append((n, syms))
    return bad


def write_yaml(script_lines, artifact):
    y = ["jobs:", "  build:", "    script:"]
    for s in script_lines:
        y.append('      - "%s"' % s.replace('"', '\\"'))
    y.append("    artifacts:")
    y.append("      - path: %s" % artifact)
    open(os.path.join(WORK, ".robuild.yaml"), "w").write("\n".join(y) + "\n")


_HAVE_ZIP = shutil.which("zip") is not None

def zip_work():
    # The RISC OS build service needs the archive laid out exactly the way the
    # Info-ZIP `zip` tool produces it (directory entries in the right order) to
    # map Unix paths like desklib/WimpSWIs.h onto its RISC OS 'h.WimpSWIs'
    # header layout. Hand-rolling this with zipfile tripped the service's
    # extractor ("File exists: desklib"), so we shell out to `zip` -- the same
    # tool that produced the archive which compiled cleanly.
    if _HAVE_ZIP:
        fd, tmp = tempfile.mkstemp(suffix=".zip", dir="/tmp"); os.close(fd)
        os.remove(tmp)
        subprocess.run(["zip", "-rq", tmp, ".", "-x", "*.DS_Store", ".base_stamp",
                        ".build_sig"], cwd=WORK, check=True)
        data = open(tmp, "rb").read()
        os.remove(tmp)
        return data
    # Fallback (no `zip` on PATH): zipfile with dir entries emitted parent-first.
    buf = io.BytesIO(); dirs_seen = set()
    with zipfile.ZipFile(buf, "w", zipfile.ZIP_DEFLATED) as z:
        for root, subdirs, files in os.walk(WORK):
            rel_root = os.path.relpath(root, WORK).replace(os.sep, "/")
            if rel_root != "." and rel_root not in dirs_seen:
                dirs_seen.add(rel_root)
                zi = zipfile.ZipInfo(rel_root + "/"); zi.external_attr = 0o40755 << 16
                z.writestr(zi, b"")
            for f in files:
                fp = os.path.join(root, f)
                rel = os.path.relpath(fp, WORK).replace(os.sep, "/")
                z.write(fp, rel)
    return buf.getvalue()


def run_build(src_bytes, timeout, tag):
    """Run one build. Returns dict: rc, throwbacks, artifact(bytes|None),
    art_name, art_ft, completed(bool), errors(list of throwback strings)."""
    src_b64 = base64.b64encode(src_bytes).decode("ascii")
    ws = websocket.create_connection(
        SERVER, timeout=timeout + 240,
        header=["Origin: https://build.riscos.online"])
    options = [("timeout", timeout), ("ansitext", False), ("arch", ARCH)]
    oi = 0; state = "connecting"
    res = dict(rc=None, throwbacks=0, artifact=None, art_name=None, art_ft=0,
               completed=False, errors=[], service_error=None)
    t0 = time.time()

    def send(a, p): ws.send(json.dumps([a, p]))
    def pump():
        nonlocal oi, state
        if oi < len(options):
            n, v = options[oi]; oi += 1; send("option", [n, v])
        else:
            send("source", src_b64); state = "source_sent"

    try:
        while True:
            raw = ws.recv()
            if raw is None or raw == "":
                break
            if isinstance(raw, bytes):
                raw = raw.decode("utf-8", "replace")
            try: arr = json.loads(raw)
            except ValueError: continue
            if not isinstance(arr, list) or not arr: continue
            typ = arr[0]; data = arr[1] if len(arr) > 1 else None
            if typ == "welcome":
                state = "options"; pump()
            elif typ == "response":
                if state == "options": pump()
                elif state == "source_sent":
                    send("build", ""); state = "build_sent"
                elif state == "build_sent":
                    state = "compiling"
            elif typ == "error":
                if state == "options": pump()
                else:
                    log("  server error: %s" % data); break
            elif typ == "message":
                LOG.write("  %s: %s\n" % (tag, data))
                ds = data if isinstance(data, str) else str(data)
                if "Build failure" in ds or "Errno" in ds or "Traceback" in ds:
                    res["service_error"] = ds.strip()
            elif typ == "output":
                txt = data if isinstance(data, str) else str(data)
                LOG.write(txt)
                sys.stdout.write(txt); sys.stdout.flush()
            elif typ == "throwback":
                res["throwbacks"] += 1
                try:
                    s = "%s %s:%s  %s" % (data.get("severity_name") or "",
                                          data.get("filename") or "",
                                          data.get("lineno"), data.get("message") or "")
                except AttributeError:
                    s = json.dumps(data)
                if any(k in s for k in ("Serious", "Error", "error")):
                    res["errors"].append(s.strip())
            elif typ == "clipboard":
                ft = data.get("filetype", 0) if isinstance(data, dict) else 0
                b64 = data.get("data", "") if isinstance(data, dict) else ""
                res["artifact"] = base64.b64decode(b64)
                res["art_ft"] = ft
            elif typ == "rc":
                res["rc"] = data if isinstance(data, int) else 1
            elif typ == "complete":
                res["completed"] = True
                break
    finally:
        try: ws.close()
        except Exception: pass
    res["elapsed"] = time.time() - t0
    return res


def merge_artifact(art_bytes):
    """Merge the returned object artifact into work/o.

    The service returns the `o` artifact directory as a zip whose internal
    layout we don't fully control (it can carry RISC OS ,xxx filetype suffixes
    and both file and directory entries for the same name). Rather than
    extractall (which hit file/dir name collisions), flatten every FILE member
    into work/o/<leaf>, stripping any ,xxx suffix and skipping directory
    entries. All object names are unique, so flattening by leaf is safe.
    """
    odir = os.path.join(WORK, "o")
    os.makedirs(odir, exist_ok=True)
    # keep a copy of the raw artifact (in HERE, not WORK, so it isn't re-zipped
    # into the next upload) + log its layout for diagnosis
    try:
        open(os.path.join(HERE, "_last_artifact.zip"), "wb").write(art_bytes)
    except Exception:
        pass
    written = 0
    with zipfile.ZipFile(io.BytesIO(art_bytes)) as z:
        names = z.namelist()
        LOG.write("artifact: %d members; sample: %s\n" % (len(names), names[:10]))
        for zi in z.infolist():
            name = zi.filename
            if name.endswith("/"):
                continue                        # directory entry
            leaf = os.path.basename(name.rstrip("/"))
            if not leaf or leaf == ".keep":
                continue
            m = re.match(r'^(.*),[0-9A-Fa-f]{3}$', leaf)   # strip RISC OS filetype
            if m:
                leaf = m.group(1)
            with z.open(zi) as src, open(os.path.join(odir, leaf), "wb") as dst:
                shutil.copyfileobj(src, dst)
            written += 1
    LOG.write("artifact: merged %d object(s) into work/o\n" % written)


def _riscos_extra(ftype):
    """SparkFS/Acorn zip extra field carrying a RISC OS filetype+datestamp.
    Header id 0x4341 ('AC'), data = "ARC0" + load + exec + attr (all LE).
    A load address of 0xFFFtttXX marks a typed file (ttt = 12-bit filetype);
    exec/low time bits are left 0 (datestamp = 0, which is harmless)."""
    load = 0xFFF00000 | ((ftype & 0xFFF) << 8)
    execa = 0
    attr = 0x03                      # owner read+write
    data = b"ARC0" + struct.pack("<III", load, execa, attr)
    return struct.pack("<HH", 0x4341, len(data)) + data


def _riscos_filetype(arc):
    """RISC OS filetype for a path inside the app (arc uses '/' separators)."""
    name = arc.rsplit("/", 1)[-1]
    if name == "RDPClient" or arc.endswith("ClipStore/ClipStore"):
        return 0xFF8                 # Absolute
    if name == "!!DeepKeys":
        return 0xFFA                 # Module
    if name in ("!Run", "!Boot", "Connect", "LoadSound", "ConnectEx", "InstDeepK"):
        return 0xFEB                 # Obey
    if name.startswith("!Sprites") or name in ("Sprites", "Sprites22"):
        return 0xFF9                 # Sprite
    if name == "Templates":
        return 0xFEC                 # Template
    return 0xFFF                     # Text (help, messages, licence, C/H source)


def make_riscos_zip(src_dir, zip_path):
    """Zip an entire directory tree with RISC OS filetypes embedded (Acorn extra
    field) so unzipping on RISC OS restores every type -- RDPClient/ClipStore as
    Absolute, !Run/!Boot/Connect as Obey, sprites as Sprite, Templates as
    Template, !!DeepKeys as Module, the rest Text -- with no manual SetType.
    The zip file itself and junk (.DS_Store, *.bak*) are skipped. Entries are
    stored relative to src_dir, so the archive holds the distribution as
    top-level !RDPClient, DeepKeys, ConnectEx, Licence, ... just like the
    original."""
    zabs = os.path.abspath(zip_path)
    zf = zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED)
    for root, dirs, files in os.walk(src_dir):
        dirs.sort(); files.sort()
        for fn in files:
            full = os.path.join(root, fn)
            if os.path.abspath(full) == zabs:
                continue
            if fn == ".DS_Store" or ".bak" in fn:
                continue
            arc = os.path.relpath(full, src_dir).replace(os.sep, "/")
            zi = zipfile.ZipInfo(arc)
            zi.compress_type = zipfile.ZIP_DEFLATED
            zi.external_attr = 0o644 << 16
            zi.extra = _riscos_extra(_riscos_filetype(arc))
            with open(full, "rb") as fh:
                zf.writestr(zi, fh.read())
    zf.close()
    log("Built: RISC OS zip written to %s" % zip_path)
    log("       (filetypes embedded -- unzip on RISC OS, no manual SetType).")



def assemble_built(outp):
    """Assemble a ready-to-run application in ./Built/!RDPClient.

    WHY: the app's resources (Messages, Templates, sprites, DeepKeys, ...) live
    in the source !RDPClient, not here in "Claude outputs". Copying only the
    linked binary to the Pi leaves those stale (old menu text, old version).
    This step produces one self-contained folder to copy across.

    WHAT: refresh a full copy of the source !RDPClient into ./Built/!RDPClient
    (so Messages/Templates edits propagate), then drop the freshly linked binary
    in as the 'RDPClient' file. The binary and resources therefore always match.
    Set the 'RDPClient' file's type to Absolute (&FF8) after copying to the Pi.
    """
    app_src = os.path.normpath(os.path.join(HERE, "..", "rdpclientsrc",
                                            "RDPClient", "!RDPClient"))
    built   = os.path.join(HERE, "Built")
    app_dst = os.path.join(built, "!RDPClient")
    if not os.path.isdir(app_src):
        log("Built: source !RDPClient not found at %s -- skipped." % app_src)
        return
    os.makedirs(built, exist_ok=True)
    if os.path.isdir(app_dst):
        shutil.rmtree(app_dst)
    shutil.copytree(app_src, app_dst)
    # Keep the deployable app clean: drop build source and junk (the original
    # !RDPClient ships without c/h/rdesktop; GPL source stays in app_base.zip).
    for d in ("c", "h", "rdesktop"):
        dp = os.path.join(app_dst, d)
        if os.path.isdir(dp):
            shutil.rmtree(dp)
    for root, ds, fs in os.walk(app_dst):
        for fn in fs:
            if fn == "Makefile" or fn == ".DS_Store" or ".bak" in fn:
                try: os.remove(os.path.join(root, fn))
                except OSError: pass
    shutil.copyfile(outp, os.path.join(app_dst, "RDPClient"))
    # Bundle the distribution extras the app needs to run (DeepKeys module, which
    # the app requires installed in !Boot) plus the original docs/licence, if a
    # dist_extras/ folder is present next to this script. They sit beside
    # !RDPClient in Built, matching the original distribution layout.
    extras = os.path.join(HERE, "dist_extras")
    if os.path.isdir(extras):
        for entry in sorted(os.listdir(extras)):
            src = os.path.join(extras, entry)
            dst = os.path.join(built, entry)
            if os.path.isdir(src):
                if os.path.isdir(dst): shutil.rmtree(dst)
                shutil.copytree(src, dst)
            else:
                shutil.copyfile(src, dst)
        log("Built: bundled dist_extras (DeepKeys, docs) beside !RDPClient.")
    # One RISC OS zip of the whole distribution (Built/), types embedded.
    make_riscos_zip(built, os.path.join(built, "RDPClient_app.zip"))
    log("Built: assembled %s" % app_dst)
    log("       (full app resources + fresh binary, matched). Copy this whole")
    log("       !RDPClient folder to the Pi; set its 'RDPClient' file to")
    log("       Absolute (&FF8).")


def main():
    setup, compiles, finals = load_plan()
    fresh_work()

    # Build signature: if the compile commands change (e.g. adding -apcs
    # 3/32bit), previously-built objects are stale and MUST be rebuilt. Wipe
    # work/o when the signature changes so we never link stale objects.
    import hashlib
    SIG_VERSION = "7-0921-clean"   # bump to force a full clean rebuild of all objects
    sig = hashlib.sha1((SIG_VERSION + "\n" + "\n".join(c["cmd"] for c in compiles)
                        + "\n" + "\n".join(finals)).encode()).hexdigest()
    sigpath = os.path.join(WORK, ".build_sig")
    prev_sig = open(sigpath).read().strip() if os.path.exists(sigpath) else None
    # If no signature is recorded yet but objects already exist (built by an
    # older driver that didn't stamp one), we can't trust they match the current
    # flags -> treat as a mismatch and rebuild.
    if prev_sig is None and any(n != ".keep" for n in os.listdir(os.path.join(WORK, "o"))):
        prev_sig = "<unknown>"
    if prev_sig is not None and prev_sig != sig:
        od = os.path.join(WORK, "o")
        for n in os.listdir(od):
            if n != ".keep":
                try: os.remove(os.path.join(od, n))
                except OSError: pass
        log("Compile flags changed -> cleared work/o; all objects will rebuild.")
    open(sigpath, "w").write(sig)

    done = built_objs()
    todo = [c for c in compiles if c["out"] not in done]
    log("Total compiles: %d | already built: %d | remaining: %d"
        % (len(compiles), len(compiles) - len(todo), len(todo)))

    slice_n = START_SLICE
    i = 0
    while i < len(todo):
        batch = todo[i:i + slice_n]
        names = ", ".join(c["out"].replace("o.", "") for c in batch)
        log("\n=== compile chunk: %d..%d of %d  (slice=%d)  [%s]"
            % (i + 1, i + len(batch), len(todo), slice_n, names))
        script = list(setup) + [c["cmd"] for c in batch]
        write_yaml(script, "o")
        r = run_build(zip_work(), CHUNK_TIMEOUT, "cc")
        log("  -> rc=%s throwbacks=%s elapsed=%.0fs completed=%s"
            % (r["rc"], r["throwbacks"], r.get("elapsed", 0), r["completed"]))

        if r["rc"] == 0 and r["artifact"]:
            merge_artifact(r["artifact"])
            got = built_objs()
            missing = [c["out"] for c in batch if c["out"] not in got]
            if missing:
                log("  !! chunk rc=0 but objects missing: %s" % missing)
                log("     (treating as failure — stopping)"); return 2
            i += len(batch)
            # chunk succeeded comfortably: allow a modest grow
            if r.get("elapsed", 999) < CHUNK_TIMEOUT * 0.55 and slice_n < 16:
                slice_n += 2
            continue

        # not a clean success
        if r["service_error"]:
            log("\n!!! SERVICE ERROR (not a compile problem): %s" % r["service_error"])
            log("This is a build-service/packaging failure, not your source.")
            log("Stopped — shrinking the slice won't help. See buildapp.log.")
            return 4

        if r["errors"]:
            log("\n!!! COMPILE ERROR in chunk — first messages:")
            for e in r["errors"][:25]:
                log("    " + e)
            log("\nStopped. See buildapp.log for the full compiler output.")
            log("Fix the source above, then re-run (built objects are kept).")
            return 1

        # wall-cap only: build ran but did not finish under the requested budget
        capish = (r["rc"] == 124) or (not r["completed"]) or (r.get("elapsed", 0) >= CHUNK_TIMEOUT * 0.9)
        if capish and slice_n > MIN_SLICE:
            slice_n = max(MIN_SLICE, slice_n // 2)
            log("  wall-cap suspected (rc=%s, elapsed=%.0fs). Shrinking slice to %d and retrying."
                % (r["rc"], r.get("elapsed", 0), slice_n))
            continue
        log("  Chunk failed (rc=%s, no throwbacks, no service error). Stopping — see buildapp.log."
            % r["rc"])
        return 3

    # ---- final: libraries + link ----
    log("\n=== final chunk: build tabd/pane2 libraries + link RDPClient")
    script = list(setup) + finals
    write_yaml(script, "out")     # capture the 'out' DIRECTORY (service returns
                                  # a directory artifact's contents; a bare file
                                  # path comes back empty). finals copy RDPClient
                                  # into out/ before this is captured.
    r = run_build(zip_work(), CHUNK_TIMEOUT, "link")
    log("  -> rc=%s throwbacks=%s elapsed=%.0fs completed=%s"
        % (r["rc"], r["throwbacks"], r.get("elapsed", 0), r["completed"]))
    if r["errors"]:
        log("\n!!! LINK/LIBFILE errors:")
        for e in r["errors"][:40]:
            log("    " + e)
    if r["rc"] == 0 and r["artifact"]:
        # artifact is the RDPClient absolute; the service wraps single-file
        # artifacts. If it's a zip, unpack; else write raw.
        data = r["artifact"]
        outp = os.path.join(HERE, "RDPClient")
        if data[:2] == b"PK":
            with zipfile.ZipFile(io.BytesIO(data)) as z:
                # find the RDPClient member (ignore dirs and ,xxx filetype suffix)
                member = None
                for zi in z.infolist():
                    if zi.filename.endswith("/"):
                        continue
                    leaf = os.path.basename(zi.filename.rstrip("/"))
                    leaf = re.sub(r',[0-9A-Fa-f]{3}$', '', leaf)
                    if leaf == "RDPClient":
                        member = zi.filename; break
                if member:
                    with z.open(member) as src, open(outp, "wb") as dst:
                        shutil.copyfileobj(src, dst)
                else:
                    open(outp + ".zip", "wb").write(data)
                    log("  artifact members: %s" % z.namelist()[:12])
                    log("  artifact saved as RDPClient.zip (RDPClient member not found)")
                    log("\nDONE (see RDPClient.zip)."); return 0
        else:
            open(outp, "wb").write(data)
        bad = chunk5_guard()
        if bad:
            log("")
            log("!!! CHUNK-5 GUARD FAILED -- object(s) import a C99 printf-family")
            log("    symbol that pulls SharedCLibrary stub chunk 5 (ROM CLib 5.34")
            log("    cannot initialise it) -> the 'SWI &5DC34' launch crash:")
            for n, syms in bad:
                log("      o.%s  imports  %s" % (n, ", ".join(syms)))
            log("    Fix: replace the snprintf/vsnprintf call with a bounded vsprintf")
            log("    wrapper (cm_snprintf / ce_snprintf / kh_snprintf), delete that")
            log("    object from work/o so it recompiles, then rebuild.")
            log("    The linked binary is UNSAFE and has NOT been finalised.")
            return 5
        log("chunk-5 guard: OK -- no linked object imports snprintf/vsnprintf.")
        assemble_built(outp)
        _run_consistency_check()
        log("\nDONE.  Linked binary written to: %s  (%d bytes, filetype &%03x)"
            % (outp, os.path.getsize(outp), r["art_ft"]))
        log("Drop it into !RDPClient as the 'RDPClient' file (type &FF8, Absolute).")
        return 0
    log("\nLink did not produce RDPClient (rc=%s). See buildapp.log." % r["rc"])
    return 1


def _run_consistency_check():
    """Release guardrail: verify the repo / scratch / rdpclientsrc trees agree
    (version strings, History, !Help, Clipboard, app_base.zip, plan flags).
    Non-fatal -- prints its report; a FAIL means fix before committing/releasing."""
    script = os.path.join(HERE, "consistency.py")
    if not os.path.isfile(script):
        return
    try:
        log("")
        log("--- release consistency check (consistency.py) ---")
        r = subprocess.run([sys.executable, script], cwd=HERE,
                           capture_output=True, text=True)
        for line in (r.stdout or "").rstrip().split("\n"):
            log(line)
        if r.returncode != 0:
            log("*** CONSISTENCY FAILED -- do NOT commit/release until green ***")
    except Exception as e:
        log("consistency check skipped: %s" % e)


if __name__ == "__main__":
    sys.exit(main())
