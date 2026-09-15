# Local DDE build of !RDPClient (on RISC OS)

An alternative to the online build (`build/buildapp.py`): build !RDPClient with a
**local DDE** (Norcroft) straight on a RISC OS 5 machine - e.g. a Raspberry Pi
4B. Two ways, same result:

- **`MkRDP`** - an Obey file that runs the whole build top to bottom.
- **`build.py`** - a Python script that does the same but **incrementally**
  (skips objects already built) and stops on the first error with its log.

Both are generated from `build/plan.json` + `linkvia`, so they compile and link
exactly like the online build - with two flags added back:

## Why -memaccess and -Otime are added here

The online build service's Norcroft 5.18 does **not** support `-memaccess`, so it
was removed there and unaligned accesses were fixed in the source instead. A
**full local DDE Norcroft does support it**, so this build adds
**`-memaccess -L22-S22-L41`** back (the compiler emits alignment-safe code, as
the original 0.88 build did - see the 0.85 History entry), and with that in place
**`-Otime`** is safe to re-enable too. This is also the friendlier choice for
running the result under **RPCEmu**, which is less tolerant of unaligned access.
(The source-level alignment fixes from 0.90.1/0.90.2 remain and do no harm.)

## Prerequisites

- The **DDE** installed. The scripts call the DDE's **`!SetPaths`** for you, so
  `C:`/`TCPIPLibs:` are set up automatically - you just tell them where the DDE
  is (once): edit **`DDE$Dir`** near the top of `MkRDP`, and **`DDE_DIR`** near
  the top of `build.py`, to your DDE's path (default placeholder:
  `SDFS::RISCOSpi.$.Programming.DDE`). If your DDE is already booted at startup
  (paths already set), delete the two `DDE$Dir`/`Obey ...!SetPaths` lines in
  `MkRDP` and set `DDE_DIR = None` in `build.py`. 32-bit stubs used at link:
  `C:o.stubs-32`, `TCPIPLibs:o.socklib5-32`, `TCPIPLibs:o.inetlib-32`.
- The RDPClient **source tree** unpacked, containing `c.`, `rdesktop.c.`, `h.`,
  `tabd.`, `pane2.`, `openssl.`, `hacklib.` and `desklib.` (with the 32-bit
  library as `desklib.o.DeskLib` - your DeskLib32).
- Put **`MkRDP`, `build.py`, `linkvia` and `libviaPane` in the build ROOT**
  (the directory that holds `c.` and `rdesktop.`) - the link reads `linkvia`
  and `libviaPane` from the current directory.

## Run it

Obey (full build):

```
*Dir <build root>
*Obey MkRDP
```

Python (incremental; run in a TaskWindow so output scrolls):

```
*Dir <build root>
*Python BuildRDP
```

`build.py` skips any object already present in `o.`; delete `o.` (or a single
`o.<name>`) to force a recompile. It prints the failing file's log and stops on
the first error.

## RISC OS filetypes (after transferring from the Mac/GitHub)

RISC OS uses `.` as a directory separator, so **do not keep the `.py` name** on
RISC OS - rename `build.py` to something dotless such as `BuildRDP`, filetype
**Text (&FFF)**. Set `MkRDP` to filetype **Obey (&FEB)** so it runs on
double-click. `linkvia`/`libviaPane` are Text.

## Output

The build writes **`out.RDPClient`** as an Absolute (&FF8). Drop it into
`!RDPClient` in place of the shipped binary. (`MkRDP`/`build.py` set the type for
you.)

## Keeping in step with the online build

These files are generated from `build/plan.json`. If the compile set or flags
change, regenerate them rather than hand-editing, so the local DDE build and the
online build stay identical (bar the `-memaccess`/`-Otime` additions).
