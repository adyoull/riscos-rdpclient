#!/usr/bin/env python3
"""
dumpasm.py -- compile ONE source to ARM assembly on build.riscos.online and
save the listing locally, so we can inspect exactly what Norcroft emits.

Reuses buildapp.py's connection/build machinery. Run it from the same folder
as buildapp.py / app_base.zip:

    python3 dumpasm.py           # -> writes mcs_asm.txt

By default it compiles rdesktop/c/mcs (o.rd_mcs) with the SAME flags the real
build uses, but with -S (assembly) instead of -c (object).
"""
import io, zipfile, os, sys
import buildapp as B

# The real mcs compile flags, with -c -> -S and object -> assembly output.
CMD = ("cc -S -apcs 3/32bit -Wp -DWITH_OPENSSL -DWITH_RDPSND "
       "-DGETPID_IS_MEANINGLESS -DNO_SYS_TYPES_H -DOPENSSL_NO_BIO -DOPENSSL_NO_MD2 "
       "-DOPENSSL_NO_MD4 -DOPENSSL_NO_RIPEMD -DOPENSSL_NO_DES -DOPENSSL_NO_RC2 "
       "-DOPENSSL_NO_RC5 -DOPENSSL_NO_BF -DOPENSSL_NO_CAST -DOPENSSL_NO_IDEA "
       "-DOPENSSL_NO_MDC2 -DOPENSSL_NO_AES -DCRYPTO_NO_LOCKING -throwback "
       "-I,C:,TCPIPLibs: rdesktop.c.mcs -o s.mcs")

SETUP = [
    "Set DeskLib$Path @.desklib.",
    "Set OpenSSLLib$Path @.openssl.",
    "Set HackLib$Path @.hacklib.",
    "Set C$Path <C$Path>,@.openssl.,@.hacklib.",
    "CDir s",
]
OUT = os.path.join(B.HERE, "mcs_asm.txt")

def main():
    B.fresh_work()
    B.write_yaml(SETUP + [CMD], "s")     # capture the 's' (assembly) directory
    r = B.run_build(B.zip_work(), B.CHUNK_TIMEOUT, "asm")
    print("rc=%s completed=%s throwbacks=%s" % (r["rc"], r["completed"], r["throwbacks"]))
    for e in r["errors"][:20]:
        print("  ", e)
    data = r["artifact"]
    if not data:
        print("No artifact returned -- see output above."); return 1
    if data[:2] == b"PK":
        z = zipfile.ZipFile(io.BytesIO(data))
        member = None
        for zi in z.infolist():
            if zi.filename.endswith("/"):
                continue
            leaf = os.path.basename(zi.filename.rstrip("/"))
            if leaf in ("mcs", "s.mcs") or leaf.endswith(".s") or "mcs" in leaf:
                member = zi.filename; break
        if member is None and z.namelist():
            member = [n for n in z.namelist() if not n.endswith("/")][0]
        if member:
            open(OUT, "wb").write(z.read(member))
            print("wrote %s (%d bytes) from member %s" % (OUT, os.path.getsize(OUT), member))
        else:
            print("artifact members:", z.namelist()[:20])
    else:
        open(OUT, "wb").write(data)
        print("wrote %s (%d bytes)" % (OUT, os.path.getsize(OUT)))
    return 0

if __name__ == "__main__":
    sys.exit(main())
