#!/usr/bin/env python3
import json, re
plan = json.load(open("plan.json"))
compiles = plan["compiles"]
outs = [c["out"] for c in compiles]   # unified o.<name>

# remap a linkvia token to unified name
def remap(tok):
    tok = tok.strip()
    if not tok: return None
    if tok.startswith("o."):            return tok                    # c/* objects already o.NAME
    if tok.startswith("rdesktop.o."):   return "o.rd_" + tok.split(".")[-1]
    if tok == "tabd.TabDialogL":        return "o.TabDialogL"
    if tok == "pane2.FixPane2":         return "o.FixPane2"
    if tok == "HackLib:o.HackLib":      return "o.hl_HackLib"
    if tok == "C:o.flexlib":            return "o.FlexShim"
    if tok == "C:o.Stubs":              return "C:o.stubs-32"
    if tok == "TCPIPLibs:o.Socklib5":  return "TCPIPLibs:o.socklib5-32"
    if tok == "TCPIPLibs:o.Inetlib":   return "TCPIPLibs:o.inetlib-32"
    return tok  # DeskLib:/OpenSSLLib:/C: libs unchanged

orig_link = open("appbuild/linkvia").read().split("\n")
new_link = []
for t in orig_link:
    r = remap(t)
    if r is not None: new_link.append(r)
# RISC OS `link -via` reads a line at a time and needs `-o` and its filename on
# the SAME line (a lone `-o` line errors "No argument to -o"; libfile's -via is
# more lenient, which is why libviaPane worked split). Merge them.
merged = []
i = 0
while i < len(new_link):
    if new_link[i] == "-o" and i + 1 < len(new_link):
        merged.append("-o " + new_link[i + 1]); i += 2
    else:
        merged.append(new_link[i]); i += 1
new_link = merged
open("appbuild_u_linkvia","w").write("\n".join(new_link) + "\n")

# pane2 lib via: -c o.FixPane2 then all o.pn_*
pn = [o for o in outs if o.startswith("o.pn_")]
libpane = ["-c", "o.FixPane2"] + pn
open("appbuild_u_libviaPane","w").write("\n".join(libpane) + "\n")

# final steps (unified)
finals = [
  "libfile -c o.TabDialogL o.tabd_TabDialog",
  "libfile -via libviaPane",
  "link -via linkvia",
]
plan["finals"] = finals
json.dump(plan, open("plan.json","w"), indent=1)

print("link entries:", len(new_link), "| pane2 objs:", len(pn))
print("--- new linkvia (head) ---")
print("\n".join(new_link[:6]))
print("--- new linkvia (tail libs) ---")
print("\n".join(new_link[-8:]))
print("--- libviaPane (head) ---")
print("\n".join(libpane[:5]))
