# !RDPClient — RISC OS RDP client (32-bit, 0.90)

A RISC OS port of **rdesktop 1.6.0** (Remote Desktop / RDP client), rebuilt
**32-bit for RISC OS 5** (Raspberry Pi) on the **build.riscos.online** cloud
compiler, with **mouse scroll-wheel support** added. Base port 0.88 by Andrew
Sellors; scroll-wheel work © 2026 Andrew Youll. Licensed under the **GNU GPL v2**
(with the OpenSSL linking exemption) — see `LICENSE`.

This repository is designed for a **100% reproducible build**: everything needed
to reproduce the exact working binary is here, and the single source of truth is
`app/!RDPClient/` — the build input (`build/app_base.zip`) is *generated* from it,
never hand-edited.

---

## Repository layout

```
app/!RDPClient/        THE SOURCE OF TRUTH — the complete RISC OS application:
    c/  h/             app C sources / headers (Norcroft C89, already hoisted)
    rdesktop/c,h       the rdesktop protocol layer (with the 3 critical fixes)
    Messages Templates !Run !Boot !Sprites Sprites … ClipStore/   runtime resources
build/                 the reproducible build kit:
    buildapp.py        build driver (chunked build on build.riscos.online)
    package.py         regenerates app_base.zip from app/ + libs (run after edits)
    app_base.zip       GENERATED build input (source bundle) — do not hand-edit
    plan.json          per-file compile command lines + final link
    hoist.py fixlit.py typeset.txt   C99→C89 transform used by package.py
    genfinal.py        regenerates the link/lib "via" files from plan.json
    builddesklib.py    rebuilds DeskLib 32-bit from source (produces DeskLib32)
    DeskLib32          prebuilt 32-bit DeskLib (used in place of the shipped one)
    libs/              prebuilt libraries + link scripts bundled into app_base.zip
                       (desklib, openssl, hacklib, pane2, tabd, linkvia, libviaPane)
dist_extras/           bundled with the app so a fresh install works:
    DeepKeys/          the DeepKeys module the app requires in !Boot (+ installer)
    ConnectEx Licence History ReadFirst   original-distribution files
docs/                  REBUILD_RECIPE.md, codechanges.md, DEVELOPER_addendum.md,
                       CHANGELOG.md  (see "Documentation" below)
LICENSE                GNU GPL v2 (+ OpenSSL exemption) — the app's licence
```

Build artefacts (`build/work/`, `build/Built/`, `build/_pkg/`, `*.stale*`,
`__pycache__/`) are git-ignored.

### RISC OS filetypes — the `,xxx` suffix

Git/GitHub don't store RISC OS filetypes, so every RISC OS file under
`app/!RDPClient/` and `dist_extras/` carries a **`,xxx` filetype suffix** in its
name (RISC OS convention): e.g. `!Run,feb` (Obey), `Templates,fec` (Template),
`!Sprites,ff9` (Sprite), `!!DeepKeys,ffa` (Module), `ClipStore,ff8` (Absolute),
and source/text as `,fff` (Text). Cloning onto RISC OS through a filer that
decodes `,xxx` restores every type automatically. The build tooling is
suffix-aware, so the suffixes never reach the compiler or the deployed app:

- `package.py` **strips** the suffix when building `app_base.zip` (the compiler
  sees plain leafnames like `c/Display`).
- `buildapp.py`'s deploy zip **strips** the suffix from the archive name and uses
  it as the **authoritative filetype** embedded in `Built/RDPClient_app.zip`
  (so that zip also unzips with correct types, suffix-free, on RISC OS).

Build tooling (`build/*.py`, `plan.json`) and `docs/` are host-side files and are
not suffixed.

---

## Requirements

- **A machine with plain internet access** and Python 3. The build talks to
  `wss://build.riscos.online` over a WebSocket. (Note: a sandbox/VPN that
  proxies or blocks that host will fail — use a normal network.)
- `python3 -m pip install websocket-client`
- The **build service itself** (build.riscos.online) provides the compiler and
  hosted libraries — this is the one external dependency and is what makes the
  build reproducible: **Norcroft cc 5.18 (JRF)**, the linker, and the
  `C:` / `TCPIPLibs:` libraries (`stubs-32`, `socklib5-32`, `inetlib-32`).
- A **RISC OS 5 machine** (e.g. Raspberry Pi) to run the result, with **DeepKeys
  2.04+** installed in `!Boot` (bundled in `dist_extras/`; run `InstDeepK` once).

No RISC OS hardware or cross-compiler is needed to *build* — only to run.

---

## Reproducible build — quick start

```
cd build
python3 -m pip install websocket-client      # once
python3 package.py                            # app/ (+ libs) -> app_base.zip
python3 buildapp.py                           # compile on the service + link + assemble
```

`buildapp.py` finishes by writing **`build/Built/RDPClient_app.zip`** — the whole
application as a RISC OS zip with every filetype embedded (Absolute / Obey /
Sprite / Template / Module / Text). Copy that zip to the Pi and unzip it there
(SparkFS or `unzip`): you get `!RDPClient` (binary already Absolute — no manual
SetType) plus `DeepKeys`. Run `DeepKeys.InstDeepK` once if DeepKeys isn't already
installed, then run `!RDPClient`.

`package.py` is the anti-divergence step: **always** run it after editing
anything under `app/!RDPClient/{c,h,rdesktop}`, so `app_base.zip` (the bundle the
compiler sees) is regenerated from the source of truth instead of drifting from
it. It applies the mandatory C89 transform, folds in `libs/`, and rewrites
`app_base.zip`.

### How the build works (buildapp.py)

- Extracts `app_base.zip` into `build/work/` (only when the zip changed).
- Compiles, in wall-clock-capped chunks, only the objects **missing** from
  `build/work/o/` (incremental); order/flags come from `plan.json`. A full clean
  build is ~40 minutes; incremental rebuilds are a couple of minutes.
- `SIG_VERSION` in `buildapp.py` (hashed with the compile flags) clears
  `work/o/` to force a full rebuild when flags change.
- Force one file to recompile: remove/stale its object —
  `mv work/o/<Name> work/o/<Name>.stale` (delete isn't always available).
- Links against the **32-bit** libraries (see "32-bit assurance").

---

## 32-bit assurance (avoiding the accidental 26-bit build)

32-bit is forced explicitly, not left to a macro that can misfire:

- Every one of the 72 compile lines in `plan.json` carries **`-apcs 3/32bit`**.
- No app/rdesktop/DeskLib source branches on a 26/32-bit macro, and there are no
  assembler steps that could default to 26-bit.
- The link (`build/libs/linkvia`) uses the **32-bit** libraries: `C:o.stubs-32`
  (not 26-bit `C:o.Stubs`), `TCPIPLibs:o.socklib5-32`, `TCPIPLibs:o.inetlib-32`,
  and the rebuilt **`DeskLib32`** (buildapp.py drops it in over the shipped one).

---

## The three critical 32-bit / Norcroft fixes (in the source)

These do not reproduce in the original 26-bit binary; without them the 32-bit
build launches but shows a **blank screen** (or fails on launch). They are
**already applied in this repo's source** (they previously lived only in cached
objects, which is why a clean rebuild used to regress):

1. **`app/!RDPClient/rdesktop/h/parse`** — all `in_/out_uint16/32` macros forced
   to alignment-safe **byte-by-byte** reads (the `#if` guards are `#if 0`), never
   the unaligned `*(uintNN*)p` fast path. (Fixes garbled MCS channel → blank.)
2. **`rdesktop/c/mcs`** + **`rdesktop/c/secure`** — a file-scope
   `volatile int g_diag_mcs_chan` carries the parsed MCS channel from
   `mcs_recv()` to `sec_recv()`, which reloads it before the routing test.
   Works around a Norcroft codegen bug that dropped the channel value.
3. **DeskLib `Wimp.h` `wimp_colourflags`** — the `unsigned int : 1` bitfield
   group is replaced with a single `unsigned char extflags;` (Norcroft padded
   the struct to 12 bytes and corrupted `numicons`). This fix lives in the
   **DeskLib source** and is baked into the prebuilt `build/DeskLib32`.

See `docs/DEVELOPER_addendum.md` (sections A–C) for the full diagnosis.

---

## Scroll-wheel feature (0.90)

Rolling the wheel over the remote desktop scrolls the **remote**, in every
display mode, with no scroll-bar furniture:

- `app/!RDPClient/c/Mouse` reads the wheel directly from the OS —
  **`OS_Pointer 2` (SWI &64)**, the "alternate positioning device" — each poll,
  and forwards notches to the remote as RDP wheel events
  (`MOUSE_FLAG_BUTTON4`/`BUTTON5`).
- An iconbar-menu **Scroll** submenu offers **Invert** and **Slower/Faster**;
  settings persist to `<Choices$Write>.RDPClientWheel`.
- Speed/direction are tunable in `c/Mouse` (`WHEEL_SPEED_*`) or via the menu.

---

## DeskLib (prebuilt)

`build/DeskLib32` is a 32-bit DeskLib built from source with the same compiler
(the official prebuilt DeskLib is GCC-built and misses symbols Norcroft's stubs
need, and it also needs fix #3 above). To rebuild it reproducibly, use
`build/builddesklib.py` against a DeskLib source tree (not included here — it is
large and external; see the script header). For normal app builds you do not
need to rebuild DeskLib; the committed `DeskLib32` is used as-is.

---

## Documentation

- **`docs/REBUILD_RECIPE.md`** — the full from-scratch build guide.
- **`docs/codechanges.md`** — running log of every source/build change.
- **`docs/DEVELOPER_addendum.md`** — deep diagnosis of the runtime bugs and the
  debugging technique. (Its scroll-wheel section D describes an earlier,
  abandoned WindowScroll approach — the shipping design is the `OS_Pointer` read
  above; treat section D as history.)
- **`docs/CHANGELOG.md`** — user-facing change history.

---

## Build → deploy checklist

1. `cd build && python3 package.py && python3 buildapp.py`
2. Copy `build/Built/RDPClient_app.zip` to the Pi; unzip it there.
3. If DeepKeys isn't installed: run `DeepKeys.InstDeepK` once.
4. Run `!RDPClient`. Info box should read `0.90`; wheel scrolls the remote.
