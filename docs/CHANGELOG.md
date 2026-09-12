# !RDPClient — Changelog

RISC OS port of rdesktop 1.6.0 (Andrew Sellors), rebuilt 32-bit on the
`build.riscos.online` online build service, with a mouse scroll-wheel feature and
a series of fixes needed to make a Norcroft / RISC OS 5 build actually run.

**Licence:** !RDPClient is free software under the **GNU General Public License
v2** (inherited from rdesktop, with the OpenSSL linking exemption). All
modifications below are released under the same GPL, and the complete
corresponding **modified source** is distributed alongside the binary (this
repository). Files changed for the scroll-wheel feature carry a modification
copyright notice: **scroll-wheel modifications (C) 2026 Andrew Youll**.

Dates are ISO (YYYY-MM-DD).

---

## 0.90.1 — 2026-09-12

Point release making the 32-bit online build run under RISC OS 5 **CPU
alignment checking** (the ARMv7 default, e.g. on a Raspberry Pi 4), plus 8-bit
buffer and crash-diagnostics fixes.

### Background

The original 0.88 binary was built with **`-memaccess -L22-S22-L41`** (see the
0.85 History entry) which makes the compiler emit alignment-safe code. The
online build service's **Norcroft cc 5.18** does not support `-memaccess`, so
several unaligned data aborts ("type 20") that the flag used to paper over now
had to be fixed in the source itself.

### Fixed

- **Data aborts ("type 20") under CPU alignment checking.** Norcroft compiled
  reads of some 16-bit *globals* into unaligned word loads (`LDR`/`LDR [rn,#2];
  MOV rn,LSR #16` instead of `LDRH`), which abort when alignment checking is ON.
  Fixed by widening the offending globals to a full word — they only ever carry
  16 bits on the wire, so protocol output is unchanged:
  - `g_mcs_userid` (`rdesktop/c/mcs`) — aborted in `mcs_connect`.
  - `g_server_rdp_version` (`rdesktop/c/secure`) — aborted in
    `rdp_send_logon_info`.
  - `samplewidth` (`c/Sound`) — widened proactively (same class; sound path).

  Located with a startup crash-postmortem capture, a link map, and per-file
  `-S` assembly dumps (`build/dumpasm.py`). **24-bit display now connects with
  CPU alignment ON**, so no `!Config` change is needed.
- **Removed `-Otime`** from every compile (it encouraged folding byte reads into
  unaligned word loads); `-memaccess` is unavailable on Norcroft 5.18.
- **Crash/postmortem capture restored.** `c/Die` redirects the C runtime's error
  output to `<Wimp$ScrapDir>.RDPClient.Error` from startup and leaves the
  hardware-fault signals uncaught, so an abort records registers, the aborting
  address and a backtrace — which is how the alignment faults above were pinned
  down.
- **8-bit pixel-translation buffer overrun.** The table was written into a fixed
  256-byte buffer, but its real size is `2^(source depth)` bytes, so a deep
  remote session on an 8-bit screen overran it and corrupted the heap. Now
  allocated to the size ColourTrans requires (`c/Display`) and freed on teardown.

### Known issues / next (→ 0.90.2)

- **8-bit (256-colour) is not yet fully working.** With alignment ON it aborts
  in `process_palette` (another unaligned access, under investigation); with
  alignment OFF a separate draw-time heap issue remains. 8-bit work is deferred
  to **0.90.2**. **24-bit is the recommended mode and is fully working.**
- Scroll wheel needs a real wheel device. Under **RPCEmu** the emulated machine
  must be set to 256MB RAM on RISC OS 5.30/5.31 for the wheel to be delivered.

### Licence

- Alignment fixes touch `rdesktop/c/mcs`, `rdesktop/c/secure`, `rdesktop/c/rdp`
  and `c/Sound`; each carries a **(C) 2026 Andrew Youll** modification notice.

---

## 0.90 — 2026-09-10

First fully working 32-bit rebuild from the online build service, with mouse
scroll-wheel support. Getting here required fixing several bugs that only appear
when the port is compiled with the service's **Norcroft cc 5.18 (JRF)** compiler
for **32-bit RISC OS 5** — none of which were present in the original 26-bit
binary — plus making the build fully reproducible.

### Added

- **Mouse scroll-wheel support.** Rolling the wheel over the remote desktop
  scrolls the **remote**, in **every display mode** (Window, Full Window and
  Full Screen), with no scroll-bar furniture required. The wheel is read
  directly from the OS via **`OS_Pointer 2` (SWI &64)** — the "alternate
  positioning device" — in `c/Mouse`: each poll diffs the accumulated wheel Y and
  forwards notches to the RDP server as wheel input (`MOUSE_FLAG_BUTTON4` = up /
  `MOUSE_FLAG_BUTTON5` = down). Reading from the OS means the feature is
  independent of the Wimp's WindowScroll behaviour, so it works identically
  windowed and full-screen.

- **Scroll settings menu.** The iconbar menu gains a **Scroll** submenu with
  **Invert** (reverse wheel direction) and **Slower / Faster** (notch speed).
  Settings persist across sessions to `<Choices$Write>.RDPClientWheel` and are
  reloaded at startup.

- Version bumped to **0.90**.

### Fixed — runtime correctness on Norcroft / 32-bit / RISC OS 5

- **Blank/black RDP display (root cause: unaligned + wrong-endian stream reads).**
  `rdesktop/h/parse` selected a "fast" macro path that reads 16/32-bit protocol
  fields with unaligned pointer casts (`*(uint16*)p`). On ARM these return
  rotated garbage at odd offsets, and the big-endian variant was wrong on a
  little-endian machine anyway. The MCS channel id (`0x03EB` = 1003) was read as
  garbage, so every packet carrying the desktop was routed to the wrong channel
  and discarded — the session connected but never drew. Fixed by forcing the
  **unconditional byte-by-byte, endian-correct** macros (the `#if` guards are set
  to `#if 0`).

- **Channel still misread after the parse fix (Norcroft codegen bug).**
  Even with correct byte-by-byte macros, `mcs_recv()`'s `uint16 *channel` output
  parameter was not reliably written back into the caller's local inside
  `sec_recv()` (the compiler cached a stale value). Worked around by having
  `mcs_recv()` also store the channel in a file-scope **`volatile int`**
  (`g_diag_mcs_chan`) and having `sec_recv()` reload that value immediately
  before the routing test. `volatile` forces a real memory load; `int` avoids the
  miscompiled 16-bit load path.

- **"Not enough memory to copy template" fatal on launch (struct-layout bug).**
  DeskLib's `wimp_colourflags` used five `unsigned int : 1` bitfields after
  `char` fields; Norcroft word-aligns the bitfield unit, making the struct 12
  bytes instead of 8, which pushed `numicons` to the wrong offset and made
  `Template_Clone` request a huge allocation. Fixed by replacing the bitfield
  group with a single `unsigned char extflags;` (no code references the
  individual flags). This fix lives in the DeskLib source and is baked into the
  prebuilt 32-bit `DeskLib32`.

### Changed — build / packaging

- Full **32-bit** build: **`-apcs 3/32bit`** on every one of the 72 compile
  lines, linked against the 32-bit libraries (`stubs-32`, `socklib5-32`,
  `inetlib-32`) and a **DeskLib rebuilt from source** with the same compiler (the
  official prebuilt DeskLib is GCC-built and references symbols Norcroft's stubs
  don't provide). 32-bit is forced explicitly rather than left to a macro that
  can misfire — the cause of the earlier accidental 26-bit build.

- **Optimisation: `-Otime`** added to all 72 compiles (the build previously used
  no optimisation). The `volatile` channel mirror keeps the MCS/secure path
  correct under the optimiser.

- **DIAG diagnostics removed.** The per-packet `DIAG …` connection prints are
  gone, so 0.90 is a clean, quiet build.

- Sources pass through an automatic **C99 → C89** transform (declaration
  hoisting + compound-literal lifting) so the C89-only service compiler accepts
  them; the original `c/Display` contained mid-block declarations Norcroft
  rejects.

- **100% reproducible build.** The single source of truth is `app/!RDPClient/`;
  the build input (`build/app_base.zip`) is *generated* from it by
  `build/package.py`, never hand-edited. The three critical fixes above now live
  in the source (they previously survived only in cached object files, so a clean
  rebuild used to regress to a blank screen).

- **RISC OS filetypes preserved on GitHub.** Every RISC OS file carries a `,xxx`
  filetype suffix in its name so types survive clone/checkout; the build tooling
  strips the suffix before compiling and re-embeds the correct type in the deploy
  zip.

- **Deploy as a RISC OS zip.** The build finishes by writing
  `Built/RDPClient_app.zip` — the whole distribution with every filetype embedded
  (Absolute / Obey / Sprite / Template / Module / Text) — so unzipping on RISC OS
  restores all types with no manual `SetType`. The **DeepKeys** module the app
  requires is bundled.

### Licence / attribution

- Base port 0.88 remains credited to **Andrew Sellors**. GPL modification-
  copyright notices — **(C) 2026 Andrew Youll** — added to the files changed for
  the scroll-wheel feature (`c/Mouse`, `h/Mouse`), preserving the original
  authors' copyright. Modified source is shipped with the binary to satisfy the
  GPL.

### Known issues

- If the remote scrolls too far or too little per notch, adjust the speed from
  the **Scroll → Slower / Faster** menu (or `WHEEL_SPEED_*` in `c/Mouse`).

---

## 0.88 — 2010-04-05 (original)

- Original RISC OS rdesktop 1.6.0 port by Andrew Sellors (26-bit).
