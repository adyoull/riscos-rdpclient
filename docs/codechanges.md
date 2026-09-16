# !RDPClient — Code Changes Log

A running log of source/build changes, newest first. Each entry says **what**,
**which files**, and **why**. Companion to CHANGELOG.md (user-facing) and
DEVELOPER_addendum.md / REBUILD_RECIPE.md (background).

---

## 2026-09-11 (later) — 0.90.1

### Fix: heap overrun in 8-bit screen modes (pixel translation table)
- **`c/Display` / `h/Display`** — `build_pixtranstable()` wrote a ColourTrans
  pixel-translation table into a fixed `char pixtransbuffer[256]` field of the
  malloc'd `display_block`. The table is `2^(source bpp)` bytes, so a >8bpp
  remote plotted to an 8bpp screen wrote up to 65536 bytes into 256 → malloc
  heap corruption ("not enough memory / heap overwritten"), only in shallow
  (8-bit) screen modes (deep modes need no table → `requiredsize==0`, early
  return). Fixed: `pixtransbuffer`/`pointer_pixtransbuffer` are now `char *` with
  a size, `realloc`'d to `requiredsize` on demand and `free`'d in
  `Display_Destroy`. Same treatment for the pointer-sprite table.

### Fix: unaligned-access aborts ("type 20")
- **`plan.json`** — removed **`-Otime`** from all 72 compile command lines (it
  let the optimiser fold byte reads into unaligned word loads).
- NOTE: `-memaccess -L22-S22-L41` (the ARMv7-compat flag that stops Norcroft
  emitting unaligned `LDR`/`STR`) was tried but is **rejected by the build
  service's Norcroft 5.18** ("bad option '-memaccess': ignored") — it only
  exists in newer DDE compilers. So it is NOT used; any residual unaligned
  access must be fixed in the source (as was already done in `rdesktop/h/parse`).
  0.90 added `-Otime`, which lets Norcroft fold byte/halfword reads into
  **unaligned word loads**; on RISC OS 5 builds that don't fix up unaligned
  access (reported on an RPi4 with 5.3x — "type 20" abort at startup, and an
  RPi1 needing ARMv5 compatibility mode) the unaligned load faults. Reverting to
  no optimisation matches 0.88's known-good codegen. Removing the flag changes
  buildapp.py's SIG hash, forcing a full clean rebuild of every object.
- **`!RDPClient/Messages`** — `info.version: 0.90.1 (11-Sep-2026)`.
- The `volatile` channel mirror (fix C) stays; it was there to defeat a codegen
  bug independent of `-O`, so it is still correct with optimisation off.

## 2026-09-11

### GitHub repository + RISC OS filetype suffixes
- Assembled this standalone, reproducible git repo (`app/` source of truth,
  `build/` kit, `dist_extras/`, `docs/`, `LICENSE`, `README.md`).
- `build/package.py` regenerates `build/app_base.zip` from `app/` + `build/libs`;
  verified it reproduces the known-good bundle **byte-for-byte**.
- Added **`,xxx` RISC OS filetype suffixes** to every file under `app/!RDPClient/`
  and `dist_extras/` (117 files), so GitHub/clone preserves types. Made the
  pipeline suffix-aware: `package.py` strips the suffix for `app_base.zip`
  (plain leafnames), and `buildapp.py`'s `make_riscos_zip` strips it from the
  archive name and uses it as the authoritative embedded filetype. Verified the
  deploy zip emits base names with correct types (!Run &FEB, RDPClient &FF8,
  Templates &FEC, sprites &FF9, !!DeepKeys &FFA, ClipStore &FF8, text &FFF).
- `build/buildapp.py` paths adjusted for the repo layout (`../app/!RDPClient`,
  `../dist_extras`).

## 2026-09-10

### Build reproducibility — critical fixes put back into SOURCE
The two runtime fixes below existed **only inside the cached `work/o` object
files** (compiled from an earlier fixed source that had since been overwritten by
`app_base.zip` re-extraction). The source in `app_base.zip` was pristine, so any
full/clean rebuild would have regressed to a **blank screen**. Re-applied to
source (authoritative text from DEVELOPER_addendum.md B & C):

- **`rdesktop/h/parse`** — fix B. Forced the alignment-safe **byte-by-byte**
  read/write macros unconditionally: changed both
  `#if defined(L_ENDIAN)&&!defined(NEED_ALIGN)` / `#if defined(B_ENDIAN)...`
  guards to `#if 0`, so the unaligned `*(uintNN*)p` "fast" path is never used on
  ARM.
- **`rdesktop/c/mcs`** — fix C. Added file-scope `volatile int g_diag_mcs_chan`;
  in `mcs_recv()` store `g_diag_mcs_chan = *channel;` right after
  `in_uint16_be(s, *channel);`.
- **`rdesktop/c/secure`** — fix C. Added `extern volatile int g_diag_mcs_chan;`
  and, in `sec_recv()`, reload `channel = (uint16) g_diag_mcs_chan;` immediately
  before the `if (channel != MCS_GLOBAL_CHANNEL)` routing test. Defeats the
  Norcroft codegen bug that dropped the parsed channel (→ desktop packets
  misrouted → blank screen).

(Fix A — DeskLib `Wimp.h` `wimp_colourflags` → single `unsigned char extflags` —
lives in the prebuilt `DeskLib32`, which a full app rebuild does not recompile,
so it is unaffected.)

### Optimisation
- **`plan.json`** — added `-Otime` (Norcroft speed optimisation) to all 72
  compile command lines (previously none had any `-O`). Changing the compile
  commands changes buildapp.py's `SIG` hash, which clears `work/o` and forces a
  full rebuild — so this build recompiles everything from the fixed source with
  optimisation on. The `volatile` in fix C keeps mcs/secure correct under the
  optimiser.
- **DIAG debug output removed** — the per-packet `error("DIAG …")` prints existed
  only in the stale objects; the clean source has none, so the optimised full
  rebuild is automatically DIAG-free.
- Safety net before the full rebuild: `work/o` copied to
  `work_o_backup_preOtime/`; `plan.json.bak_preOtime` kept. To revert: restore
  both and rebuild.

### Scroll-wheel feature (final design)
- **`rdesktop`/display untouched; wheel read directly from the OS.**
- **`c/Mouse`** — `poll_wheel()` (called from `Mouse_Poll`) reads `OS_Pointer 2`
  (SWI &64, the scroll-wheel "alternate device"), diffs the accumulated Y, and
  forwards notches to the remote as `MOUSE_FLAG_BUTTON4/5` via `send_wheel_notch()`.
  Works in all display modes, needs no scroll-bar furniture. Added
  `wheel_invert`/`wheel_speed`, the settings API
  (`Mouse_SetWheelInvert`/`Mouse_GetWheelInvert`/`Mouse_WheelSpeedFaster`/
  `Mouse_WheelSpeedSlower`/`Mouse_GetWheelSpeed`) and persistence to
  `<Choices$Write>.RDPClientWheel` (read in `Mouse_Init`).
- **`h/Mouse`** — declarations for the new API.
- **`c/Display`** — reverted to **pristine 0.88** (all earlier WindowScroll
  scroll-bar experiments removed).
- **GPL** — modification-copyright notices (© 2026 Andrew Youll) added to
  `c/Mouse` and `h/Mouse`.

### Scroll settings menu
- **`c/RDPClient`** — build a code "Scroll" submenu
  (`Menu_New("Scroll","Invert|Slower,Faster")`), attach with `Menu_AddSubMenu`,
  handle Invert toggle + Slower/Faster; refresh the Invert tick on menu open.
  New defines `mainmenu_SCROLL 7` / `mainmenu_QUIT 8` (Quit shifted) and
  `scrollmenu_INVERT/SLOWER/FASTER`.
- **`!RDPClient/Messages`** — `menu.main`: added `Scroll` before `Quit`.
  **Important:** NO `>` prefix — `>` sets the Wimp "notifysub" flag, which sends a
  submenu *warning* instead of auto-opening the attached menu (that was the
  "submenu won't open" bug). A plain entry + `Menu_AddSubMenu` auto-opens.

### Version
- **`!RDPClient/Messages`** — `info.version: 0.90 (10-Sep-2026)` (date refreshed;
  runtime resource, no rebuild needed).

### Packaging / delivery (`buildapp.py`)
- `assemble_built()` — after linking, build `Built/!RDPClient` (full app
  resources + fresh binary, source/junk pruned), bundle `dist_extras/` (DeepKeys
  module the app needs in `!Boot`, plus ConnectEx/Licence/History/ReadFirst)
  beside it.
- `make_riscos_zip()` — write `Built/RDPClient_app.zip`: the whole distribution as
  a **RISC OS zip with filetypes embedded** (Acorn/SparkFS extra field), so
  unzipping on RISC OS restores every type (Absolute/Obey/Sprite/Template/Module/
  Text) with no manual SetType. Types verified against the original 0.88 zip.

### Reverted / dropped
- Author-field credit in the Info box — implemented then **reverted** at user
  request (RDPClient.c `icon_AUTHOR`/`Popup_proginfo` change and the
  `info.author` Messages line removed). Author line stays "Andrew Sellors".


---

## 0.92.0 — Connection manager

New module **`ConnMgr`** (`c/ConnMgr`, `h/ConnMgr`): the saved-connection data
model. A `connmgr_connection` record (name, server, port, user, password,
domain, width/height, depth, display mode, experience/speed, and the sound /
clipboard / compression / RDP4 flags) round-trips to a runnable Obey file that
calls the `Connect` launcher with the equivalent command-line options.
`ConnMgr_BuildObey` / `ConnMgr_ParseObey` build and parse that line;
`ConnMgr_Save` / `ConnMgr_Load` read and write it (filetype Obey &FEB);
`ConnMgr_List` enumerates `<Choices$Write>.RDPClient.Connections` with OS_GBPB.
Sentinels: `width == -1` -> `-g screen`, `depth == -1` -> `-a screen`
("Same as RISC OS"). The plan gains one compile (`o.ConnMgr`) and the linkvia
gains `o.ConnMgr`.

New window template **`ConnEdit`** added to `Templates` (generated binary, not
hand-edited): labelled writable fields (name, server, port, user, password,
domain), pop-up-menu display fields (resolution, colour, display mode, speed)
each with a `gright` menu button, option buttons (sound, clipboard, compression,
old-server) as Wimp radio icons (button type 11), Cancel/Save/Connect action
buttons, and a red plain-text-password note. The password field uses the `D*`
validation for on-screen masking.

**`c/ConnEdit`** (`c/ConnEdit` + `h/ConnEdit`) — the editor and manager UI, its
own compile unit (`o.ConnEdit`, added to plan.json and linkvia). It exposes
`ConnEdit_Init` / `ConnEdit_RebuildMenu(mainmenu, item)` / `ConnEdit_MenuSelect`;
`c/RDPClient` keeps only those three hooks (init in setup, rebuild on iconbar
menu open, dispatch in the menu handler):
- `connedit_*` — create the `ConnEdit` window, populate it from / read it back
  into a record, four pop-up menus (resolution/colour/display/speed) handled in
  `connedit_menuchoice`, a key handler (`connedit_key`) giving Tab / Shift-Tab /
  Return / cursor navigation between the writable fields, and Save/Connect.
- `connmenu_*` — the dynamic **Connections** iconbar submenu, rebuilt on each
  menu open from `ConnMgr_List`, with **New connection…** plus a shared
  **Connect / Edit / Delete** submenu per connection; Delete confirms via
  `Wimp_ReportError` (OK/Cancel). Iconbar `menu.main` gains a `Connections`
  entry (submenu).

*(Originally these lived in `c/RDPClient` to keep UI iterations incremental; moved
out into `c/ConnEdit` for 0.92.0 once the feature was stable — a pure code move,
no behaviour change.)*

**`c/RDesktop`** — display-mode fix and banner: the display mode is now emitted
as `-D window` / `-D fullwindow` / `-D fullscreen` (the old `-f` only set
bring-to-front and never changed the mode). The startup `usage()` banner credits
the original RISC OS port (Andrew Sellors, 2004-2010, orac2.demon.co.uk) and the
32-bit update (Andrew Youll, 2026, github.com/adyoull/riscos-rdpclient), and
documents the `-v` protocol-trace option.
