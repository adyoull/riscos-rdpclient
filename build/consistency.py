#!/usr/bin/env python3
"""Release-consistency guard for !RDPClient.

The same logical files live in THREE trees and have silently diverged before:
  * scratch build dir : "Claude outputs/"  (plan.json, app_base.zip, dist_extras/, work/)
  * git repo          : "riscos-rdpclient/"
  * resource source   : "../rdpclientsrc/RDPClient/!RDPClient/"  (Messages, !Help)

This script compares every duplicated file/version and exits non-zero on ANY
mismatch. Run it (buildapp.py also runs it at the end of every build) and make it
PASS before committing or releasing. It changes nothing -- it only reports.
"""
import io, os, re, sys, json, hashlib, zipfile

HERE = os.path.dirname(os.path.abspath(__file__))
def P(*a): return os.path.join(HERE, *a)

REPO = P("riscos-rdpclient")
SRC  = os.path.normpath(P("..", "rdpclientsrc", "RDPClient", "!RDPClient"))

def read(path, enc="latin-1"):
    with io.open(path, "r", encoding=enc, errors="replace") as f: return f.read()
def sha(path):
    h=hashlib.sha256()
    with open(path,"rb") as f:
        for b in iter(lambda:f.read(65536), b""): h.update(b)
    return h.hexdigest()
def zip_member_sha(zpath, member):
    with zipfile.ZipFile(zpath) as z:
        return hashlib.sha256(z.read(member)).hexdigest()

fails=[]; checks=[]
def ok(label):    checks.append(("PASS", label))
def bad(label,d): checks.append(("FAIL", label)); fails.append(label+((" -- "+d) if d else ""))

def first(pattern, text, flags=0):
    m=re.search(pattern, text, flags); return m.group(1) if m else None

# ---- canonical version from the repo Messages ----
try:
    canon = first(r"info\.version:\s*(\S+)", read(P(REPO,"app","!RDPClient","Messages,fff")))
except Exception as e:
    print("CANNOT READ repo Messages,fff: %s"%e); sys.exit(2)
if not canon:
    print("CANNOT parse canonical version from repo Messages,fff"); sys.exit(2)
print("Canonical version (repo Messages,fff): %s\n" % canon)

# each entry: label, path, extractor(text)->version-or-None
VERSION_SOURCES = [
 ("repo   Messages,fff",           P(REPO,"app","!RDPClient","Messages,fff"),        lambda t: first(r"info\.version:\s*(\S+)", t)),
 ("src    Messages (Info box)",    os.path.join(SRC,"Messages"),                     lambda t: first(r"info\.version:\s*(\S+)", t)),
 ("repo   README.md (title)",      P(REPO,"README.md"),                              lambda t: first(r"\(32-bit,\s*([0-9.]+)\)", t)),
 ("repo   README.md (latest)",     P(REPO,"README.md"),                              lambda t: first(r"latest release is \*\*([0-9.]+)\*\*", t)),
("repo   CHANGELOG.md (top)",     P(REPO,"docs","CHANGELOG.md"),                    lambda t: first(r"^## +([0-9][0-9.]*)", t, re.M)),
 ("repo   History,fff (top)",      P(REPO,"dist_extras","History,fff"),              lambda t: first(r"^\s*\d+-\w+-\d+\s+([0-9.]+)\b", t, re.M)),
 ("scratch dist_extras/History",   P("dist_extras","History"),                       lambda t: first(r"^\s*\d+-\w+-\d+\s+([0-9.]+)\b", t, re.M)),
 ("repo   !Help,fff (top entry)",  P(REPO,"app","!RDPClient","!Help,fff"),           lambda t: first(r"^([0-9]+\.[0-9]+(?:\.[0-9]+)?)\s*\(\d", t, re.M)),
 ("src    !Help (top entry)",      os.path.join(SRC,"!Help"),                        lambda t: first(r"^([0-9]+\.[0-9]+(?:\.[0-9]+)?)\s*\(\d", t, re.M)),
]
for label, path, extract in VERSION_SOURCES:
    if not os.path.exists(path): bad("version: "+label, "missing file"); continue
    v=extract(read(path))
    if v is None: bad("version: "+label, "could not parse")
    elif v!=canon: bad("version: "+label, "%s != %s"%(v,canon))
    else: ok("version: "+label+" = "+v)

# ---- History: scratch == repo (byte identical) ----
try:
    if read(P("dist_extras","History")) == read(P(REPO,"dist_extras","History,fff")): ok("History: scratch == repo")
    else: bad("History: scratch dist_extras/History != repo dist_extras/History,fff","content differs")
except Exception as e: bad("History compare", str(e))

# ---- Messages line: scratch(src) == repo (whole version line) ----
try:
    a=first(r"(info\.version:[^\n]*)", read(os.path.join(SRC,"Messages")))
    b=first(r"(info\.version:[^\n]*)", read(P(REPO,"app","!RDPClient","Messages,fff")))
    ok("Messages: src == repo version line") if a==b else bad("Messages version line","src '%s' != repo '%s'"%(a,b))
except Exception as e: bad("Messages compare", str(e))

# ---- Licence text: GPL v3, and byte-identical across every shipped copy ----
# Guards the slip where the licence is updated in one tree but not the
# build-source trees that assemble_built copies into the release zip.
LICENCE_COPIES = [
    ("repo dist_extras Licence,fff", P(REPO,"dist_extras","Licence,fff")),
    ("repo LICENSE",                 P(REPO,"LICENSE")),
    ("src  !RDPClient Licence",      os.path.join(SRC,"Licence")),
    ("scratch dist_extras/Licence",  P("dist_extras","Licence")),
]
try:
    canon_lic = read(P(REPO,"app","!RDPClient","Licence,fff"))
    if ("version 3 of the License" in canon_lic) or ("General Public License version 3" in canon_lic):
        ok("Licence: repo app Licence,fff is GPL v3")
    else:
        bad("Licence: repo app Licence,fff", "does not carry GPL v3 text")
    for label, path in LICENCE_COPIES:
        if not os.path.exists(path):
            bad("Licence: "+label, "missing file")
        elif read(path) == canon_lic:
            ok("Licence: "+label+" == repo app Licence,fff")
        else:
            bad("Licence: "+label+" != repo app Licence,fff", "content differs")
except Exception as e:
    bad("Licence compare", str(e))

# ---- OpenSSL / SSLeay notice: the file ships, is complete, is in sync, and the
# two required acknowledgements appear in the shipped docs (SSLeay licence
# requires the acknowledgement in documentation or at startup). ----
ACK_OSSL = "This product includes software developed by the OpenSSL Project"
ACK_EAY  = "This product includes cryptographic software written by Eric Young"
OSSL_COPIES = [
    ("repo    dist_extras OpenSSL-Licence,fff", P(REPO,"dist_extras","OpenSSL-Licence,fff")),
    ("scratch dist_extras OpenSSL-Licence",     P("dist_extras","OpenSSL-Licence")),
    ("Built   OpenSSL-Licence",                 P("Built","OpenSSL-Licence")),
]
try:
    canon_ossl = read(OSSL_COPIES[0][1])
    need = ["OpenSSL License", "Original SSLeay License", ACK_OSSL, ACK_EAY]
    miss = [s for s in need if s not in canon_ossl]
    if miss:
        bad("OpenSSL notice: repo OpenSSL-Licence,fff", "missing: %s" % ", ".join(miss))
    else:
        ok("OpenSSL notice: repo OpenSSL-Licence,fff complete (both licences + both acknowledgements)")
    for label, path in OSSL_COPIES[1:]:
        if not os.path.exists(path):
            bad("OpenSSL notice: "+label, "missing file")
        elif read(path) == canon_ossl:
            ok("OpenSSL notice: "+label+" == repo")
        else:
            bad("OpenSSL notice: "+label+" != repo", "content differs")
except Exception as e:
    bad("OpenSSL notice compare", str(e))
# acknowledgement reproduced in the shipped !Help and ReadFirst
for label, path in [
    ("repo   !Help,fff",            P(REPO,"app","!RDPClient","!Help,fff")),
    ("src    !Help",                os.path.join(SRC,"!Help")),
    ("repo   dist_extras ReadFirst,fff", P(REPO,"dist_extras","ReadFirst,fff")),
    ("scratch dist_extras ReadFirst",    P("dist_extras","ReadFirst")),
]:
    try:
        t = read(path)
        if ACK_OSSL in t:
            ok("OpenSSL notice: acknowledgement in "+label)
        else:
            bad("OpenSSL notice: "+label, "OpenSSL acknowledgement missing")
    except Exception as e:
        bad("OpenSSL notice: "+label, str(e))

# ---- tracked source files: identical across work / repo / both app_base.zip ----
SOURCES = [
    ("work/c/Clipboard",      "app/!RDPClient/c/Clipboard,fff",      "c/Clipboard"),
    ("work/c/Status",         "app/!RDPClient/c/Status,fff",         "c/Status"),
    ("work/c/AcornSSLIf",     "app/!RDPClient/c/AcornSSLIf,fff",     "c/AcornSSLIf"),
    ("work/h/AcornSSLIf",     "app/!RDPClient/h/AcornSSLIf,fff",     "h/AcornSSLIf"),
    ("work/h/AcornSSL",       "app/!RDPClient/h/AcornSSL,fff",       "h/AcornSSL"),
    ("work/rdesktop/c/iso",   "app/!RDPClient/rdesktop/c/iso,fff",   "rdesktop/c/iso"),
    ("work/rdesktop/c/tcp",   "app/!RDPClient/rdesktop/c/tcp,fff",   "rdesktop/c/tcp"),
    ("work/rdesktop/c/mcs",   "app/!RDPClient/rdesktop/c/mcs,fff",   "rdesktop/c/mcs"),
    ("work/rdesktop/h/proto", "app/!RDPClient/rdesktop/h/proto,fff", "rdesktop/h/proto"),
    ("work/rdesktop/c/secure", "app/!RDPClient/rdesktop/c/secure,fff", "rdesktop/c/secure"),
    ("work/rdesktop/c/licence","app/!RDPClient/rdesktop/c/licence,fff","rdesktop/c/licence"),
    ("work/rdesktop/c/rdp",    "app/!RDPClient/rdesktop/c/rdp,fff",    "rdesktop/c/rdp"),
    ("work/c/Display",         "app/!RDPClient/c/Display,fff",         "c/Display"),
]
for wrel, rrel, member in SOURCES:
    name = wrel.split("/")[-1]
    try:
        wsha = sha(P(*wrel.split("/")))
    except Exception as e:
        bad("source %s: work" % name, str(e)); continue
    try:
        rsha = sha(os.path.join(REPO, *rrel.split("/")))
        ok("source %s: work == repo" % name) if wsha==rsha else bad("source %s: work vs repo" % name, "sha differ")
    except Exception as e:
        bad("source %s: repo" % name, str(e))
    for label, z in [("scratch app_base.zip", P("app_base.zip")), ("repo build/app_base.zip", P(REPO,"build","app_base.zip"))]:
        try:
            zs = zip_member_sha(z, member)
            ok("source %s: %s == work" % (name, label)) if zs==wsha else bad("source %s in %s" % (name, label), "sha != work")
        except KeyError:
            bad("source %s in %s" % (name, label), "%s not in zip" % member)

# ---- app_base.zip: scratch == repo ----
try:
    ok("app_base.zip: scratch == repo build") if sha(P("app_base.zip"))==sha(P(REPO,"build","app_base.zip")) else bad("app_base.zip scratch vs repo","sha differ")
except Exception as e: bad("app_base.zip compare", str(e))

# ---- plan.json: both carry -za1 and -Otime on every compile ----
def plan_flags(path):
    plan=json.load(open(path)); cs=plan.get("compiles",[])
    n=len(cs); za=sum("-za1" in c["cmd"] for c in cs); ot=sum("-Otime" in c["cmd"] for c in cs)
    return n, za, ot
for label, path in [("scratch plan.json", P("plan.json")), ("repo build/plan.json", P(REPO,"build","plan.json"))]:
    try:
        n,za,ot=plan_flags(path)
        if za==n and ot==n and n>0: ok("plan flags: %s (-za1 & -Otime on all %d)"%(label,n))
        else: bad("plan flags: %s"%label,"compiles=%d za1=%d Otime=%d"%(n,za,ot))
    except Exception as e: bad("plan flags: %s"%label, str(e))

# ---- report ----
print("Release consistency:")
for st,label in checks: print("  [%s] %s"%(st,label))
print()
if fails:
    print("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
    print("  RELEASE CONSISTENCY: FAIL (%d) -- fix before committing/releasing" % len(fails))
    for f in fails: print("   - "+f)
    print("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
    sys.exit(1)
print("  RELEASE CONSISTENCY: PASS -- all trees agree on %s" % canon)
sys.exit(0)
