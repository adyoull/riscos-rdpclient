# !RDPClient — Changelog

RISC OS port of rdesktop 1.6.0 (Andrew Sellors), rebuilt 32-bit on the
`build.riscos.online` online build service, with a scroll-wheel feature and a
series of fixes needed to make a Norcroft/RISC OS 5 build actually run.

**Licence:** !RDPClient is free software under the **GNU General Public License**
(inherited from rdesktop). All modifications below are released under the same
GPL, and the complete corresponding **modified source** is distributed alongside
the binary (in `app_base.zip` and the `RDPClient-buildkit` tree). Files changed
for the scroll-wheel feature carry a modification copyright notice:
**mouse-wheel modifications (C) 2026 Andrew Youll**.

Dates are ISO (YYYY-MM-DD).

---

## 0.90 — 2026-09 (in progress)

First fully working 32-bit rebuild from the online build service. Getting here
required fixing several bugs that only appear when the port is compiled with the
service's **Norcroft cc 5.18 (JRF)** compiler for **32-bit RISC OS 5**, none of
which were present in the original 26-bit binary.

### Added
- **Mouse scroll-wheel support** — wheel movement over the remote desktop is
  forwarded to the RDP server as wheel input (`MOUSE_FLAG_BUTTON4` = up /
  `MOUSE_FLAG_BUTTON5` = down) by `Mouse_Wheel()` in `c/Mouse`, gated on the
  display having input focus.

  How it works on RISC OS 5: this Pi does **not** deliver reason-10
  Scroll_Request events. Instead the **WindowScroll module auto-scrolls the
  window under the pointer** (a reason-2 Open_Window_Request) — but only if that
  window has a scroll bar and room to scroll. `OpenWindow()` intercepts that
  auto-scroll, converts the offset change into a wheel notch via `Mouse_Wheel()`,
  and **pins the local scroll offset back** so the RISC OS display never scrolls
  itself — the wheel belongs entirely to the remote desktop. The display window
  is given a phantom over-size work area so the wheel always has somewhere to
  auto-scroll (and hence be forwarded), even when the whole remote image is
  already visible.

  *(Status: **confirmed working on-device in "Window" and "Full Window" modes.**)*

- **Full-screen wheel scrolling via "Full Window"** — "Full Window" mode
  (`display_mode_FULLSCREEN`) now uses the **same furnitured, scrollable display
  window** as windowed mode, opened to **fill the screen with its title bar and
  scroll bars off the screen edges** (`open_window_fullscreen()` sets the visible
  work area to `screen_size`). This gives a clean, full-screen-looking view that
  scrolls the remote. The old borderless full-screen window had no scroll-bar
  furniture, so the WindowScroll module never auto-scrolled it and the wheel did
  nothing full-screen. Exit/return between modes is unchanged (Ctrl+Shift+D /
  iconbar menu). *(Status: **confirmed working on-device, 2026-09-10.** This is
  the recommended full-screen mode.)*

- **`WheelTest` BASIC capture test** — a standalone BBC BASIC WIMP program (needs
  no build service) that reports live, in its title bar, whether the wheel
  arrives as a reason-10 Scroll_Request or as an auto-scroll Open_Window_Request.
  Used to prove the delivery mechanism independently of the C app, Norcroft
  codegen, and the RDP session.

- Version bumped to **0.90**; `!Help` features/wishlist/version-history updated.

### Fixed — runtime correctness on Norcroft / 32-bit / RISC OS 5
- **Blank/black RDP display (root cause: unaligned + wrong-endian stream reads).**
  `rdesktop/h/parse` selected a "fast" macro path that reads 16/32-bit protocol
  fields with unaligned pointer casts (`*(uint16*)p`). On ARM these return
  rotated garbage at odd offsets, and the big-endian variant was wrong on a
  little-endian machine anyway. The MCS channel id (`0x03EB` = 1003) was read as
  garbage, so every packet carrying the desktop was routed to the wrong channel
  and discarded — the session connected but never drew. Fixed by replacing the
  conditional macros with **unconditional byte-by-byte, endian-correct** ones.
- **Channel still misread after the parse fix (Norcroft codegen bug).**
  Even with correct byte-by-byte macros, `mcs_recv()`'s `uint16 *channel` output
  parameter was not reliably written back into the caller's local inside
  `sec_recv()`'s loop (the compiler cached an uninitialised value; a plain local
  assignment was mis-compiled too). Worked around by having `mcs_recv()` also
  store the channel in a file-scope **`volatile int`** (`g_diag_mcs_chan`) and
  having `sec_recv()` use that value directly in the routing decision. `volatile`
  forces a real memory load; `int` avoids the miscompiled 16-bit load path.
- **"Not enough memory to copy template" fatal on launch (struct-layout bug).**
  DeskLib's `wimp_colourflags` used five `unsigned int : 1` bitfields after
  `char` fields; Norcroft word-aligns the bitfield unit, making the struct 12
  bytes instead of 8, which pushed `numicons` to the wrong offset (read as 24
  instead of 4) and made `Template_Clone` request a huge allocation. Fixed by
  replacing the bitfield group with a single `unsigned char extflags;` (no code
  references the individual flags).

### Changed — build/packaging
- Full **32-bit** build (`-apcs 3/32bit` on every `cc` and `objasm`), linked
  against 32-bit libraries (`socklib5-32`, `inetlib-32`, `stubs-32`) and a
  **DeskLib rebuilt from source** with the same compiler (the official prebuilt
  DeskLib is GCC-built and references symbols Norcroft's stubs don't provide).
- Sources pass through an automatic **C99 → C89** transform (declaration
  hoisting via `hoist.py` + compound-literal lifting via `fixlit.py`) so the
  C89-only service compiler accepts them. The original 0.88 `c/Display` contained
  mid-block declarations that Norcroft rejects; these are now hoisted to block
  tops before packaging. See DEVELOPER.md / BUILD_PROCESS.md.
- `flex` shifting-heap API is provided by a small malloc-backed **FlexShim**
  because the service links `C:o.flexlib` but does not supply it.

### Licence / attribution
- Added GPL modification-copyright notices — **(C) 2026 Andrew Youll** — to every
  file changed for the scroll-wheel feature (`c/Display`, `c/Mouse`, `h/Mouse`),
  preserving the original authors' copyright. Modified source is shipped with the
  binary to satisfy the GPL.

### Not done / decisions
- **"Full Screen" (single-task) wheel scrolling is not supported.** That mode
  grabs the raw screen and opens no scrollable Wimp window, so the WindowScroll
  module has nothing to auto-scroll. An experiment hosting a hidden window under
  the screen-grab was tried and reverted; **"Full Window" is used as the
  full-screen mode instead**, since it already scrolls and looks full-screen.

### Known issues
- If the remote scrolls too far or too little per notch, tune the notch count in
  `Mouse_Wheel()` or throttle the `OpenWindow()` intercept.
- The binary still carries `DIAG …` connection diagnostics (from the rdesktop
  layer); harmless, left in to avoid touching the files that hold the 32-bit
  fixes. Can be stripped for a final clean 0.90 if wanted.
- The build writes the binary to a file named `RDPClient` (RISC OS type must be
  set to `&FF8`, Absolute). Copy *that* file into `!RDPClient` — see DEVELOPER.md
  "Delivering the binary".

---

## 0.88 — 2010-04-05 (original)
- Original RISC OS rdesktop 1.6.0 port by Andrew Sellors (26-bit).
