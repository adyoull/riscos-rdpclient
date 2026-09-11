# !RDPClient — Code Changes Log

A running log of source/build changes, newest first. Each entry says **what**,
**which files**, and **why**. Companion to CHANGELOG.md (user-facing) and
DEVELOPER_addendum.md / REBUILD_RECIPE.md (background).

---

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
