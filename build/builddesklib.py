#!/usr/bin/env python3
"""
Adaptive chunked builder for a 32-bit DeskLib 2.80 library on
build.riscos.online, using the service's own Norcroft compiler so the result is
ABI/runtime-compatible with the app build (the official prebuilt DeskLib is
GCC-built and references __divsi3/__ctype_* which Norcroft's stubs don't
provide).

Compiles all 516 DeskLib objects (256 C + 260 assembler) with -apcs 3/32bit in
wall-cap-sized slices, carrying the object dir forward between builds, then
libfiles them into o.DeskLib. Output: ./DeskLib32 (drop-in 32-bit library that
buildapp.py picks up automatically).

Usage:
    python3 -m pip install websocket-client
    python3 builddesklib.py
Resumable (objects persist in deskwork/o); delete deskwork/ to start clean.
"""
import sys, os, re, json, base64, time, zipfile, shutil, io, subprocess, tempfile, hashlib

try:
    import websocket
except ImportError:
    sys.exit("Missing dependency. Run:  python3 -m pip install websocket-client")

HERE     = os.path.dirname(os.path.abspath(__file__))
PLAN     = os.path.join(HERE, "dlplan.json")
BASE_ZIP = os.path.join(HERE, "desklib_src.zip")
WORK     = os.path.join(HERE, "deskwork")
SERVER   = "wss://build.riscos.online/ws"
ARCH     = "aarch32"
CHUNK_TIMEOUT = 480
START_SLICE   = 40          # DeskLib objects are tiny/fast (~4s); big slices ok
MIN_SLICE     = 4

LOG = open(os.path.join(HERE, "builddesklib.log"), "w", encoding="utf-8", errors="replace")
def log(s=""):
    print(s, flush=True)
    try: LOG.write(s + "\n"); LOG.flush()
    except Exception: pass

_HAVE_ZIP = shutil.which("zip") is not None

def fresh_work():
    os.makedirs(WORK, exist_ok=True)
    try: stamp = str(os.path.getmtime(BASE_ZIP)) + ":" + str(os.path.getsize(BASE_ZIP))
    except OSError: stamp = "?"
    sp = os.path.join(WORK, ".base_stamp")
    prev = open(sp).read().strip() if os.path.exists(sp) else None
    if prev != stamp:
        log("Syncing DeskLib source -> deskwork/ (%s)" %
            ("first unpack" if prev is None else "source changed"))
        with zipfile.ZipFile(BASE_ZIP) as z: z.extractall(WORK)
        # source changed -> discard cached objects so everything rebuilds
        od = os.path.join(WORK, "o")
        if os.path.isdir(od):
            for n in os.listdir(od):
                if n != ".keep":
                    try: os.remove(os.path.join(od, n))
                    except OSError: pass
            log("  source changed -> cleared deskwork/o; all objects will rebuild.")
        open(sp, "w").write(stamp)
    os.makedirs(os.path.join(WORK, "o"), exist_ok=True)

def built_objs():
    od = os.path.join(WORK, "o")
    return set("o." + n for n in os.listdir(od) if n != ".keep")

def write_yaml(script_lines, artifact):
    y = ["jobs:", "  build:", "    script:"]
    for s in script_lines:
        y.append('      - "%s"' % s.replace('"', '\\"'))
    y += ["    artifacts:", "      - path: %s" % artifact]
    open(os.path.join(WORK, ".robuild.yaml"), "w").write("\n".join(y) + "\n")

def zip_work():
    if _HAVE_ZIP:
        fd, tmp = tempfile.mkstemp(suffix=".zip", dir=HERE); os.close(fd); os.remove(tmp)
        subprocess.run(["zip", "-rq", tmp, ".", "-x", "*.DS_Store", ".base_stamp"],
                       cwd=WORK, check=True)
        data = open(tmp, "rb").read(); os.remove(tmp); return data
    buf = io.BytesIO(); seen=set()
    with zipfile.ZipFile(buf, "w", zipfile.ZIP_DEFLATED) as z:
        for root, _, files in os.walk(WORK):
            rr = os.path.relpath(root, WORK).replace(os.sep, "/")
            if rr != "." and rr not in seen:
                seen.add(rr); zi=zipfile.ZipInfo(rr+"/"); zi.external_attr=0o40755<<16
                z.writestr(zi, b"")
            for f in files:
                z.write(os.path.join(root,f), os.path.relpath(os.path.join(root,f),WORK).replace(os.sep,"/"))
    return buf.getvalue()

def run_build(src_bytes, timeout, tag):
    ws = websocket.create_connection(SERVER, timeout=timeout+240,
                                     header=["Origin: https://build.riscos.online"])
    options=[("timeout",timeout),("ansitext",False),("arch",ARCH)]; oi=0; state="connecting"
    res=dict(rc=None, throwbacks=0, artifact=None, completed=False, errors=[], service_error=None)
    t0=time.time()
    def send(a,p): ws.send(json.dumps([a,p]))
    def pump():
        nonlocal oi,state
        if oi<len(options):
            n,v=options[oi]; oi+=1; send("option",[n,v])
        else:
            send("source",base64.b64encode(src_bytes).decode("ascii")); state="source_sent"
    try:
        while True:
            raw=ws.recv()
            if raw is None or raw=="": break
            if isinstance(raw,bytes): raw=raw.decode("utf-8","replace")
            try: arr=json.loads(raw)
            except ValueError: continue
            if not isinstance(arr,list) or not arr: continue
            typ=arr[0]; data=arr[1] if len(arr)>1 else None
            if typ=="welcome": state="options"; pump()
            elif typ=="response":
                if state=="options": pump()
                elif state=="source_sent": send("build",""); state="build_sent"
                elif state=="build_sent": state="compiling"
            elif typ=="error":
                if state=="options": pump()
                else: log("  server error: %s"%data); break
            elif typ=="message":
                ds=data if isinstance(data,str) else str(data); LOG.write("  %s: %s\n"%(tag,ds))
                if "Build failure" in ds or "Errno" in ds or "Traceback" in ds: res["service_error"]=ds.strip()
            elif typ=="output":
                txt=data if isinstance(data,str) else str(data); LOG.write(txt)
                sys.stdout.write(txt); sys.stdout.flush()
            elif typ=="throwback":
                res["throwbacks"]+=1
                try:
                    s="%s %s:%s  %s"%(data.get("severity_name") or "",data.get("filename") or "",
                                      data.get("lineno"),data.get("message") or "")
                except AttributeError: s=json.dumps(data)
                if any(k in s for k in ("Serious","Error","error")): res["errors"].append(s.strip())
            elif typ=="clipboard":
                res["artifact"]=base64.b64decode(data.get("data","") if isinstance(data,dict) else "")
            elif typ=="rc": res["rc"]=data if isinstance(data,int) else 1
            elif typ=="complete": res["completed"]=True; break
    finally:
        try: ws.close()
        except Exception: pass
    res["elapsed"]=time.time()-t0; return res

def merge_artifact(art):
    od=os.path.join(WORK,"o"); os.makedirs(od,exist_ok=True); w=0
    with zipfile.ZipFile(io.BytesIO(art)) as z:
        for zi in z.infolist():
            if zi.filename.endswith("/"): continue
            leaf=os.path.basename(zi.filename.rstrip("/"))
            if not leaf or leaf==".keep": continue
            leaf=re.sub(r',[0-9A-Fa-f]{3}$','',leaf)
            with z.open(zi) as s, open(os.path.join(od,leaf),"wb") as d: shutil.copyfileobj(s,d)
            w+=1
    LOG.write("artifact: merged %d object(s)\n"%w)

def main():
    plan=json.load(open(PLAN))
    setup=plan["setup"]; compiles=plan["compiles"]; objects=plan["objects"]; libname=plan["libname"]
    fresh_work()
    done=built_objs()
    todo=[c for c in compiles if c["out"] not in done]
    log("DeskLib objects: %d | already built: %d | remaining: %d"%(len(compiles),len(compiles)-len(todo),len(todo)))
    slice_n=START_SLICE; i=0
    while i<len(todo):
        batch=todo[i:i+slice_n]
        log("\n=== compile %d..%d of %d (slice=%d)"%(i+1,i+len(batch),len(todo),slice_n))
        write_yaml(list(setup)+[c["cmd"] for c in batch], "o")
        r=run_build(zip_work(),CHUNK_TIMEOUT,"cc")
        log("  -> rc=%s throwbacks=%s elapsed=%.0fs"%(r["rc"],r["throwbacks"],r.get("elapsed",0)))
        if r["rc"]==0 and r["artifact"]:
            merge_artifact(r["artifact"]); got=built_objs()
            miss=[c["out"] for c in batch if c["out"] not in got]
            if miss: log("  !! rc=0 but missing: %s"%miss[:5]); return 2
            i+=len(batch)
            if r.get("elapsed",999)<CHUNK_TIMEOUT*0.5 and slice_n<80: slice_n+=10
            continue
        if r["service_error"]: log("\n!!! SERVICE ERROR: %s"%r["service_error"]); return 4
        if r["errors"]:
            log("\n!!! COMPILE ERROR:")
            for e in r["errors"][:25]: log("    "+e)
            return 1
        if (r["rc"]==124 or not r["completed"] or r.get("elapsed",0)>=CHUNK_TIMEOUT*0.9) and slice_n>MIN_SLICE:
            slice_n=max(MIN_SLICE,slice_n//2); log("  wall-cap suspected; slice -> %d"%slice_n); continue
        log("  chunk failed (rc=%s); stopping"%r["rc"]); return 3

    log("\n=== final: libfile -> %s"%libname)
    via=["-c",libname]+objects
    open(os.path.join(WORK,"dlvia"),"w").write("\n".join(via)+"\n")
    script=list(setup)+["libfile -via dlvia","CDir out","Copy %s out.DeskLib ~C~V"%libname]
    write_yaml(script,"out")
    r=run_build(zip_work(),CHUNK_TIMEOUT,"libfile")
    log("  -> rc=%s elapsed=%.0fs"%(r["rc"],r.get("elapsed",0)))
    if r["errors"]:
        for e in r["errors"][:20]: log("    "+e)
    if r["rc"]==0 and r["artifact"]:
        outp=os.path.join(HERE,"DeskLib32")
        with zipfile.ZipFile(io.BytesIO(r["artifact"])) as z:
            mem=None
            for zi in z.infolist():
                if zi.filename.endswith("/"): continue
                if re.sub(r',[0-9A-Fa-f]{3}$','',os.path.basename(zi.filename.rstrip("/")))=="DeskLib":
                    mem=zi.filename; break
            if mem:
                with z.open(mem) as s, open(outp,"wb") as d: shutil.copyfileobj(s,d)
                log("\nDONE. 32-bit DeskLib -> %s (%d bytes)."%(outp,os.path.getsize(outp)))
                log("Now run: python3 buildapp.py  (it picks up DeskLib32 automatically).")
                return 0
            log("  DeskLib not in artifact: %s"%z.namelist()[:8])
    log("\nlibfile did not produce DeskLib (rc=%s). See builddesklib.log."%r["rc"]); return 1

if __name__=="__main__":
    sys.exit(main())
