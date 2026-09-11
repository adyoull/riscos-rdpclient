#!/usr/bin/env python3
"""
package.py -- (re)generate build/app_base.zip from the source of truth.

The single source of truth is ../app/!RDPClient (c/, h/, rdesktop/). This
script applies the mandatory C99->C89 transform (hoist.py + fixlit.py) to the
C sources, combines them with the prebuilt libraries in ./libs, and writes
app_base.zip -- the exact bundle buildapp.py compiles on build.riscos.online.

Run this after editing any source under ../app/!RDPClient/{c,h,rdesktop}:

    python3 package.py        # -> writes build/app_base.zip
    python3 buildapp.py       # compiles + links + assembles Built/

Keeping app_base.zip generated (not hand-edited) is what prevents the source
and the build input from silently diverging.
"""
import os, sys, shutil, subprocess, zipfile

HERE   = os.path.dirname(os.path.abspath(__file__))
APP    = os.path.normpath(os.path.join(HERE, "..", "app", "!RDPClient"))
LIBS   = os.path.join(HERE, "libs")
STAGE  = os.path.join(HERE, "_pkg")
OUT    = os.path.join(HERE, "app_base.zip")
PY     = sys.executable

def run(script, files):
    if files:
        subprocess.check_call([PY, os.path.join(HERE, script)] + files)

def main():
    if not os.path.isdir(APP):
        sys.exit("source app not found: %s" % APP)
    if os.path.isdir(STAGE):
        shutil.rmtree(STAGE)
    os.makedirs(STAGE)

    # 1) source: c/, h/, rdesktop/  (from the app source of truth)
    shutil.copytree(os.path.join(APP, "c"),        os.path.join(STAGE, "c"))
    shutil.copytree(os.path.join(APP, "h"),        os.path.join(STAGE, "h"))
    shutil.copytree(os.path.join(APP, "rdesktop"), os.path.join(STAGE, "rdesktop"))

    # 2) C99 -> C89 on the app C sources (Norcroft is C89-only). Idempotent.
    cfiles = [os.path.join(STAGE, "c", n) for n in sorted(os.listdir(os.path.join(STAGE, "c")))]
    run("hoist.py",  cfiles)
    run("fixlit.py", cfiles)

    # 3) prebuilt libraries + link scripts (from ./libs)
    for name in sorted(os.listdir(LIBS)):
        src = os.path.join(LIBS, name)
        dst = os.path.join(STAGE, name)
        if os.path.isdir(src):
            shutil.copytree(src, dst)
        else:
            shutil.copyfile(src, dst)

    # 4) empty object output dir (buildapp's setup also does "CDir o")
    os.makedirs(os.path.join(STAGE, "o"), exist_ok=True)
    open(os.path.join(STAGE, "o", ".keep"), "w").close()

    # 5) zip it (store paths relative to STAGE)
    if os.path.exists(OUT):
        os.remove(OUT)
    with zipfile.ZipFile(OUT, "w", zipfile.ZIP_DEFLATED) as z:
        for root, dirs, files in os.walk(STAGE):
            dirs.sort(); files.sort()
            for fn in files:
                if fn == ".DS_Store" or ".bak" in fn:
                    continue
                full = os.path.join(root, fn)
                arc  = os.path.relpath(full, STAGE).replace(os.sep, "/")
                z.write(full, arc)

    shutil.rmtree(STAGE)
    print("wrote %s (%d bytes)" % (OUT, os.path.getsize(OUT)))

if __name__ == "__main__":
    main()
