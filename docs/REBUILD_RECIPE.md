# !RDPClient 0.90 — Complete Rebuild Recipe

*A self-contained guide to rebuilding this RISC OS RDP client from scratch —
written so a fresh AI session or developer with no prior context can reproduce
the working 32-bit build, the scroll-wheel feature, and the deployable app.*

Last updated: 2026-09-10.

---

## 1. What this is

**!RDPClient** is a RISC OS port of **rdesktop 1.6.0** (an RDP / "Remote
Desktop" client). Base version **0.88** (original 26-bit port by Andrew Sellors).
This project rebuilds it **32-bit** for **RISC OS 5** on a **Raspberry Pi 4B**,
using the free cloud compiler at **build.riscos.online**, and adds **mouse
scroll-wheel support** (forwarding the wheel to the remote desktop) plus a small
settings menu. Result is version **0.90**.

Licence: **GNU GPL** (inherited from rdesktop). Modifications © 2026 Andrew
Youll. Keep the source shipping with the binary.

---

## 2. The build environment — read this first

The compiler is **Norcroft cc 5.18 (JRF)**, hosted at `wss://build.riscos.online/ws`.
It runs a real RISC OS (Pyromaniac) in the cloud. Three hard facts shape
everything:

1. **C89 ONLY.** Norcroft rejects C99 mid-block declarations and compound
   literals. Sources must be transformed to C89 before building (see §5).
2. **Reachable only from a normal network.** In this project the build was
   driven from the user's **native macOS Terminal**. A sandboxed cloud container
   and the desktop-bridge Linux VM are both proxy-blocked (and the VM has no DNS)
   — they cannot reach the build service. So: **edit/prepare anywhere, but run
   the actual build from a machine with plain internet.**
3. **Single artifact, ephemeral, ~500–650 s wall cap per build.** The driver
   (`buildapp.py`) works around the wall cap by building in chunks and carrying
   object files forward.

Dependency for the driver: `python3 -m pip install websocket-client`.

---

## 3. Inputs (the build kit)

All of these live in the working folder (in the build folder):

- `buildapp.py` — the build driver (chunked, incremental, assembles the app).
- `plan.json` — the compile/link plan (per-file `cc` command lines + final link).
- `app_base.zip` — the **source** tree the service compiles: `c/`, `h/`,
  `rdesktop/c/`, `rdesktop/h/`, the DeskLib/OpenSSL/HackLib/pane2/tabd libs, and
  `linkvia`/`libviaPane`. (It does **not** contain `work/o` objects.)
- `hoist.py`, `fixlit.py`, `typeset.txt` — the C99→C89 transform (see §5).
- `builddesklib.py` — rebuilds DeskLib 32-bit from source (see §4).
- `dist_extras/` — DeepKeys module + original docs/Licence, bundled into the
  final app (see §8).
- The **source app** at `rdpclientsrc/RDPClient/!RDPClient/` — the runnable
  `!RDPClient` (Messages, Templates, sprites, !Run/!Boot, etc.) plus its `c/`,
  `h/`, `rdesktop/` source. This is the master copy the driver edits/packages.
- The **original 0.88 distribution zip** — a proper RISC OS zip; the authoritative
  source of per-file RISC OS filetypes and the DeepKeys module.

---

## 4. Step A — Rebuild DeskLib 32-bit

The official prebuilt DeskLib is GCC-built and references symbols Norcroft's
stubs don't provide, so DeskLib must be rebuilt from source with the **same**
compiler, 32-bit.

- `builddesklib.py` drives this against the same build service, in chunks, and
  produces `DeskLib32` (the linked 32-bit library).
- `buildapp.py` automatically uses `./DeskLib32` in place of the one shipped in
  `app_base.zip` if present.

Run once; keep `DeskLib32`. Only redo if DeskLib source or the compiler changes.

---

## 5. Step B — C99 → C89 transform (MANDATORY before packaging)

Norcroft is C89. The pristine 0.88 `c/Display` in particular has **mid-block
declarations** (e.g. `update_count`, `new_coord`, `area1/2`, a `time` local) that
Norcroft rejects with errors like:

```
typedef name 'wimp_point' used in expression context
illegal left operand to '->' or '.'
expected but found 'unsigned'
```

Fix by hoisting declarations to the top of their block **before** zipping into
`app_base.zip`:

```
python3 hoist.py  c/Display c/Mouse c/RDPClient     # declaration hoist
python3 fixlit.py c/Display c/Mouse c/RDPClient     # compound-literal lift
```

- `hoist.py` is **idempotent** — verify a second pass reports 0 changes; that
  confirms the file is C89-clean.
- `c/Mouse` and `c/RDPClient` are already C89-clean (hoist = no-op); `c/Display`
  needs it. Run it on any `.c` you edit.
- `fixlit.py` converts `(Type){...}` compound literals; a no-op here but safe.

---

## 6. Step C — Package sources into app_base.zip

`app_base.zip` is what the service compiles. `buildapp.py`'s `fresh_work()`
extracts it over `work/` **only when the zip changes** (it stamps
`work/.base_stamp` with the zip's mtime+size). So to make a source edit take
effect:

1. Edit the clean source (e.g. `c/Display`).
2. Run the C89 transform on it (§5).
3. Update the entry in the zip: `zip app_base.zip c/Display c/Mouse h/Mouse ...`
   (store paths as `c/<Name>`, `h/<Name>` — RISC OS leaf names, no extension).
4. Deliver the new `app_base.zip` to the build folder.

---

## 7. Step D — Build

From the machine with internet, in the build folder:

```
python3 buildapp.py
```

How it works:

- Extracts `app_base.zip` → `work/` (if changed), keeps `work/o` objects.
- Compiles only objects **missing** from `work/o` (incremental). Order/commands
  come from `plan.json`.
- Splits compiles into chunks under the wall cap; on a wall-cap hit it halves the
  slice and retries. Real compiler errors stop the run and are printed.
- `SIG_VERSION` (a constant in `buildapp.py`) is hashed with the compile flags;
  bump it to force a **full clean rebuild** (clears `work/o`).
- Final step builds the tabd/pane2 libraries and links `RDPClient`.

**Force one changed file to recompile** (its object is cached): stale the object,
because the build VM can't delete —
`mv work/o/<Name> work/o/<Name>.stale_x`. No `Display/Mouse/RDPClient` object in
`work/o` ⇒ it recompiles. (Do this after editing that source + repackaging.)

Output: the linked `RDPClient` binary, plus `buildapp.log`.

---

## 8. Step E — Assemble the deployable app + RISC OS zip

`buildapp.py` finishes by calling `assemble_built()`:

- Copies the source `!RDPClient` into `Built/!RDPClient`, pruning build source
  (`c/ h/ rdesktop/ Makefile`) and junk so it matches the original distribution.
- Drops the freshly linked binary in as `Built/!RDPClient/RDPClient` (binary and
  resources always matched).
- Copies `dist_extras/` (DeepKeys module + docs/Licence) beside `!RDPClient`.
- Writes `Built/RDPClient_app.zip` — the whole distribution as a **RISC OS zip
  with filetypes embedded** (Acorn/SparkFS extra field 0x4341 "ARC0", load addr =
  `0xFFF00000 | (filetype<<8)`), so unzipping on RISC OS restores every type with
  no manual `SetType`.

Filetypes applied (verified against the original zip): `RDPClient` &
`ClipStore/ClipStore` = **Absolute &FF8**; `!Run`/`!Boot`/`Connect`/`LoadSound`/
`ConnectEx`/`InstDeepK` = **Obey &FEB**; `!Sprites*`/`Sprites*` = **Sprite &FF9**;
`Templates` = **Template &FEC**; `!!DeepKeys` = **Module &FFA**; everything else
(Messages, !Help, Licence, docs, source) = **Text &FFF**.

**Dependencies the app needs at runtime:** `Connect`/`LoadSound` `RMEnsure`
standard system modules (UtilityModule, CallASWI, FPEmulator, SharedCLibrary,
SharedSound, StreamManager, SharedSoundBuffer) — present on any RISC OS 5 Pi —
**plus DeepKeys 2.04**, which the app requires **installed in !Boot** (it does
not load it itself). That's why DeepKeys is bundled: run its `InstDeepK` once.

---

## 9. The critical 32-bit / Norcroft runtime fixes

These are not in `c/Display`/`c/Mouse`; they live in the rdesktop layer and
DeskLib, and are already in the source tree. If rebuilding the source from a
truly pristine rdesktop, re-apply them or the app connects but shows a **blank
screen** or fails to launch:

1. **`rdesktop/h/parse` — endian/alignment.** The "fast" macros read 16/32-bit
   fields with unaligned casts (`*(uint16*)p`) and a wrong-endian `_be` path. On
   ARM/LE this returns garbage; the MCS channel id (1003) is misread and desktop
   packets are discarded. **Fix:** make all `in_/out_uint16/32_le/be` macros
   unconditional **byte-by-byte, endian-correct**.
2. **`rdesktop/c/mcs` + `rdesktop/c/secure` — Norcroft codegen.** `mcs_recv()`'s
   `uint16*` channel out-param isn't reliably written back inside `sec_recv()`'s
   loop. **Fix:** also store the channel in a file-scope **`volatile int
   g_diag_mcs_chan`**; have `sec_recv()` read that for routing.
3. **DeskLib `Wimp.h` — `wimp_colourflags` layout.** Five `unsigned int : 1`
   bitfields after `char` fields make Norcroft pad the struct to 12 bytes (not
   8), corrupting `numicons` and causing a fatal *"not enough memory to copy
   template"* on launch. **Fix:** replace the bitfield group with a single
   `unsigned char extflags;`.

(The binary still prints `DIAG …` connection lines via the rdesktop `error()`
path; harmless, left in to avoid touching these files. Strip for a silent
release if desired.)

---

## 10. The scroll-wheel feature (0.90)

**Goal:** rolling the wheel over the remote desktop scrolls the *remote*.

**Mechanism (important):** on this Pi, RISC OS does **not** deliver reason-10
Scroll_Request events. Earlier attempts using the Wimp WindowScroll module
(giving the display window a scroll bar so it auto-scrolls, then intercepting the
Open_Window_Request) worked **only when a scroll bar is visible on screen** — so
they could not give a clean full-screen view *and* scroll. The final design
**reads the wheel directly from the OS**, which works in every display mode with
no furniture:

- **`OS_Pointer 2` (SWI &64)** returns the accumulated position of the OS
  "alternate positioning device" (the scroll wheel) in R1 (signed 32-bit Y;
  +ve = wheel away = up).
- `c/Mouse` `poll_wheel()` (called from `Mouse_Poll`, which runs each poll in all
  modes) reads it, diffs against the last value, ignores implausible jumps
  (counter wrap), and forwards each notch to the remote as an RDP wheel event —
  `MOUSE_FLAG_BUTTON4` (up) / `BUTTON5` (down) via `rdp_send_input`, using the
  current pointer mapped into the remote display.
- `c/Display` is **pristine 0.88** — no scroll-bar hacks; full-screen is the
  original clean borderless window.

**Settings menu:** an iconbar-menu **Scroll** submenu — **Invert** (tick) and
**Slower/Faster** steppers (speed = remote notches per wheel step, 1..5). Built
in code and attached with `Menu_AddSubMenu`:

- `c/Mouse` holds `wheel_invert` / `wheel_speed`, applies them in `poll_wheel`,
  exposes `Mouse_SetWheelInvert` / `Mouse_GetWheelInvert` /
  `Mouse_WheelSpeedFaster` / `Mouse_WheelSpeedSlower` / `Mouse_GetWheelSpeed`,
  and persists to `<Choices$Write>.RDPClientWheel` (read at `Mouse_Init`).
- `c/RDPClient` builds the submenu (`Menu_New("Scroll","Invert|Slower,Faster")`),
  attaches it, and handles the selections.
- **`Messages` `menu.main`** gets `Scroll` added before `Quit`. **Do NOT prefix
  it with `>`** — `>` sets the Wimp "notifysub" flag, which makes the Wimp send a
  submenu *warning* and wait for the app to open it (as `Info`/`Save` do for
  their windows). A plain `Scroll` entry + `Menu_AddSubMenu` makes the Wimp
  **auto-open** the attached submenu. (This was a real bug: with `>` the submenu
  never opened.)

Menu item text lives in `!RDPClient/Messages`; the version in `info.version`
(shown in the Info box by `Popup_proginfo` via `Msgs_Lookup`). Both are runtime
resources — editing them needs no rebuild, only the updated `Messages` shipped
with the app.

---

## 11. Deploy to the Pi

Copy `Built/RDPClient_app.zip` to the Pi and unzip it there (SparkFS, or
`unzip`). You get `!RDPClient` with all filetypes correct (binary already
Absolute) plus `DeepKeys`. Run `DeepKeys.InstDeepK` once if DeepKeys isn't
already installed in `!Boot`. Done — no manual `SetType`.

(If copying loose files instead of the zip, the binary must be set to Absolute
&FF8 and resources keep their types; the zip avoids all of that.)

---

## 12. Troubleshooting quick reference

- **Compile errors "typedef name … used in expression context" / "illegal left
  operand to '->'":** C89 hoist not applied to that source → run `hoist.py` (§5).
- **Connects but blank/black screen:** the parse-endian or mcs/secure codegen fix
  is missing (§9.1/§9.2).
- **"Not enough memory to copy template" on launch:** the `wimp_colourflags`
  struct fix is missing (§9.3).
- **A source edit doesn't take effect:** `app_base.zip` unchanged (stamp) or the
  object is cached — repackage the zip and stale `work/o/<Name>` (§7).
- **Scroll submenu appears but won't open:** `>` prefix on the `Scroll` menu item
  — remove it (§10).
- **Info box shows the old version / no Scroll menu on the Pi:** you copied only
  the binary onto a stale app — deploy the whole `Built` app / the RISC OS zip
  (§8, §11).
- **Wheel too fast/slow or inverted:** Scroll menu (Faster/Slower/Invert), or the
  `WHEEL_SPEED_*` / direction in `c/Mouse`.
