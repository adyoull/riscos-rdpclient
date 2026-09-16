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

## 0.92.0 — 2026-09-16

Adds a connection manager — create, edit, run and delete saved RDP connections
from the desktop, instead of hand-writing connection Obey files.

### Added

- **Connection manager.** The iconbar menu's **Connections** entry is a submenu
  listing every saved connection, with **New connection…** at the top. Selecting
  a connection connects to it; each also has a **Connect / Edit / Delete**
  submenu, and Delete asks for confirmation. Saved connections are stored as
  runnable Obey files in `<Choices$Write>.RDPClient.Connections`, so they stay
  standalone, double-clickable launchers. A new module `ConnMgr` (`c/ConnMgr`,
  `h/ConnMgr`) holds the connection record, its Obey build/parse/save/load and
  the directory enumeration.
- **Connection editor.** A dialog (new `ConnEdit` window template, wired in
  `c/RDPClient`) for the common settings: name, server, port, user, password,
  domain, resolution, colour depth, display mode, speed, and toggles for sound,
  clipboard (with an optional KB size limit), compression and old-server (RDP4). Resolution, colour, display mode
  and speed are chosen from pop-up menus; Tab / Return / cursor keys move between
  the writable fields. **Save** writes the connection; **Connect** launches it.
- **Password field** with on-screen masking, passed as `-p` for auto-login. The
  editor shows a note that the password is stored as plain text in the
  connection's Obey file (as RDP command-line authentication requires).
- **“Same as RISC OS”** for resolution (`-g screen`) and colour depth
  (`-a screen`), matching the current RISC OS screen mode.
- **Startup banner** now credits the original RISC OS port (Andrew Sellors,
  2004–2010) and the 32-bit update (Andrew Youll, 2026, with the GitHub
  repository), and lists the `-v` protocol-trace option.

### Fixed

- **Full-screen display mode.** A saved connection set to full screen now goes
  full screen. The display mode is emitted as `-D window` / `-D fullwindow` /
  `-D fullscreen`; the earlier `-f` only set the “bring window to front on
  keypress” behaviour and never changed the display mode.

---

## 0.91.1 — 2026-09-16

Fixes RDP-over-TLS against Microsoft Windows hosts, which dropped the connection
during setup in 0.91. Adds an optional protocol trace for field diagnostics,
clearer TLS error reporting, and a source-attribution tidy-up.

### Fixed

- **RDP-over-TLS with Windows servers (Windows 10 and 11).** Modern Windows
  enforces two Enhanced (TLS) RDP Security requirements in the client's MCS
  Connect Initial that 0.91 did not meet, and dropped the connection when they
  were wrong. Non-Windows TLS servers (xrdp, NuoRDS) do not enforce them, which
  is why only Windows was affected — it was never a TLS-version issue.

  1. **serverSelectedProtocol — anti-MITM / anti-downgrade** (`clientCoreData`,
     MS-RDPBCGR 2.2.1.3.2). The client must echo back the security protocol the
     server selected during negotiation (here PROTOCOL_SSL). Windows uses this
     as a man-in-the-middle guard: it confirms client and server agree on the
     negotiated protocol, so a MITM cannot silently downgrade the security. The
     2010-era rdesktop core omitted the field, so hardened Windows saw an
     unverifiable connection and reset it on the first PDU. This was the
     decisive missing piece.
  2. **encryptionMethods = 0 under TLS** (MS-RDPBCGR 2.2.1.3.3). Under an
     external security protocol the RDP layer carries no encryption of its own,
     so the advertised RC4 methods must be 0; the client was still sending
     40/128-bit RC4.

  Both fields are built before the TLS negotiation runs, so `mcs_connect()` now
  finalises them via `sec_finalise_mcs_data_for_tls()` once `tcp_tls_active()`
  reports TLS is up — setting serverSelectedProtocol and zeroing
  encryptionMethods — immediately before the connect PDU is sent. The non-TLS
  path is unchanged.

### Added

- **Protocol trace (`-v` / `RDPClient$Debug`).** A runtime trace of the RDP
  connection sequence (MCS setup, licensing, logon, capability exchange), off by
  default. Enable it with the `-v` command-line option or by setting the system
  variable `RDPClient$Debug` (e.g. `Set RDPClient$Debug 1` in a connection Obey
  file) to diagnose connection failures without a rebuild.

### Changed

- **Real TLS error messages.** AcornSSL failures now surface the module's own
  error text (`ssl_last_errmess()`) instead of the meaningless `errno` that
  `GETDCI4ERRNO()` yields for handshake errors.
- **SNI left off deliberately.** Setting the TLS server name via AcornSSL also
  switches on mbedTLS certificate CN/SAN verification, which makes the
  self-signed certificate Windows RDP uses by default (CN = the host's own name)
  fail the handshake when connecting by IP or a non-matching name. RDP servers
  do not select their certificate by SNI, so it is disabled (behind a build-time
  `RDP_TLS_SNI`).
- **Source attribution.** `c/AcornSSLIf` credits Colin Granville's original
  AcornSSL C wrapper and drops the integration copyright line.

---

## 0.91 — 2026-09-15

Adds **RDP-over-TLS** (Enhanced RDP Security) via the RISC OS **AcornSSL**
module, so the client can connect to servers that require TLS — including modern
Windows and TLS-configured xrdp / NuoRDS — not only legacy Standard RDP Security.

### Added

- **TLS transport via AcornSSL.** When the AcornSSL module is present the client
  advertises TLS in the RDP negotiation (X.224 Connection Request). If the server
  selects TLS, the connection is upgraded to a TLS session with
  `AcornSSL_CreateSession` before any MCS traffic, and all RDP data then flows
  through `ssl_send`/`ssl_recv`. Auto-negotiated: a server that only does standard
  security selects plain RDP and the client stays on the existing plaintext path,
  so nothing regresses.
- **AcornSSL C veneer** (`c/AcornSSLIf`, `h/AcornSSLIf`, `h/AcornSSL`) — Colin
  Granville's FTPc wrapper, re-expressed with `_kernel_swi` (the online Norcroft
  5.18 rejects the original `__asm` inline assembler).
- **Status window shows "TLS"** in the encryption field while a TLS session is
  active (new `statcon.encrypt3` message), instead of the RDP-layer
  None/Login only/All data, which is meaningless under TLS.

### Notes / limitations

- **Requires the AcornSSL module** on the RISC OS machine. Without it the client
  behaves exactly as before (standard security only) — the TLS offer is not sent.
- **The server certificate is presented for approval.** AcornSSL shows a
  dialogue for the user to accept or reject the certificate, so a self-signed
  cert can be accepted interactively (a certificate accepted with "Always" is
  remembered by AcornSSL).
- **TLS only, not NLA/CredSSP.** A server that *requires* NLA is refused with a
  clear message; set such servers to "TLS, NLA not required", or use xrdp/NuoRDS.

### Fixed

- **Null-pointer crash in `Display_Poll`.** The display poller dereferenced
  `displayblock` without a NULL check, so the client could take a data abort
  when the socket poll loop ran while no display window was allocated (a dropped
  connection, or a reconnect). Guarded to match the sibling display routines — a
  pre-existing latent bug, surfaced during TLS testing and fixed here.

### Changed files

- New `app/!RDPClient/c/AcornSSLIf` + `h/AcornSSLIf` + `h/AcornSSL`;
  `rdesktop/c/iso` (negotiation), `rdesktop/c/tcp` (TLS transport),
  `rdesktop/c/secure` (Enhanced Security — no RDP-layer encryption under TLS),
  `rdesktop/c/licence` (mark licensing complete on the server's licence result),
  `rdesktop/h/proto`, `c/Status` (+ `Messages` `statcon.encrypt3`),
  `c/Display` (Display_Poll NULL guard); `build/plan.json` (+1 compile) and
  `build/app_base.zip`.

---

## 0.90.3 — 2026-09-14

Completes two-way clipboard support: **pasting text from a RISC OS application
into the remote session now works.** (0.90.2 fixed the other direction,
server → RISC OS.)

### Fixed

- **Clipboard, RISC OS → server (paste into the remote session).** When a RISC OS
  application holds the clipboard, the client now announces **both** `CF_TEXT` and
  `CF_UNICODETEXT`. When the server requests `CF_UNICODETEXT` (the norm for xrdp,
  Windows and NuoRDS) the RISC OS Latin-1 text is up-converted to **UTF-16LE**.
  The terminator is now a **single** UTF-16 NUL: the previous code widened a
  trailing NUL already present in the RISC OS data *and* appended its own,
  producing an interior NUL that xrdp tolerated (text arrived corrupted) but that
  NuoRDS/macOS rejected outright (nothing pasted).
- **Accept clipboard data of any filetype from the RISC OS holder.** The client
  used to give up unless the holder offered plain text (`&FFF`); it now completes
  the transfer whatever type is offered, requesting text on save via the
  DataSaveAck. Word processors such as **Writer+/Fireworkz** (which offer their
  native `&D01`) are handled when they can export text; the DataLoad handler
  still verifies the delivered data really is text before anything is sent to the
  server, so a holder that can only supply its native format is rejected safely.

### Changed files

- `app/!RDPClient/c/Clipboard` — dual-format announce; `CF_UNICODETEXT` send
  conversion with a single canonical terminator; accept-any-offered-type on
  DataSave.
- `build/plan.json` — **`-Otime` re-enabled** on all compiles. 0.90.1 had
  removed it because it encouraged the unaligned word accesses behind the
  "type 20" aborts; `-za1` (0.90.2) now forces aligned code generation, so
  `-Otime` is safe again and restores the speed optimisation.

### Notes

- Confirmed working RISC OS → NuoRDS (Andrew) once the terminator was made
  canonical. On xrdp this should now paste clean text where 0.90.2 pasted
  corrupted text.
- Some styled-document apps may not supply plain text through the global
  clipboard at all; that is an app limitation, not a client bug.

---

## 0.90.2 — 2026-09-14

Release that resolves the **entire** CPU-alignment fault class at the compiler
level (including 8-bit mode) and fixes several issues seen connecting to
**xrdp** servers.

### The real alignment fix

0.90.1 fixed individual unaligned data aborts by widening globals in the source,
but 8-bit mode still aborted in `process_palette`, and the underlying cause was
general: `buildapp.py` compiles each file with its own `cc` command line that
**bypasses** the RISC OS shared-makefile defaults, so Norcroft's alignment-safe
codegen was never switched on. The fix is to pass **`-za1`** (disable unaligned
loads/stores) on every compile — the supported Norcroft 5.18 equivalent of the
old `-memaccess` behaviour. With `-za1` the compiler never emits a word
load/store at a non-word-aligned address, so the whole "type 20" abort class
disappears — **including 8-bit (256-colour) mode** — without relying on the
per-global source widenings (which remain in place, harmless, as belt-and-braces).

### Fixed

- **All remaining data aborts ("type 20") under CPU alignment checking**, via
  `-za1` on all 72 compiles in `build/plan.json`. **8-bit (256-colour) now works**
  with CPU alignment checking ON — the `process_palette` abort is gone, and the
  8-bit draw-time heap issue noted in 0.90.1 no longer reproduces.
- **xrdp: corrupted username field when no user is configured.** Connecting to an
  xrdp server without `-u <username>` passed a NULL username into
  `sec_connect` / `rdp_send_logon_info` / `licence_send_request`, which
  dereferenced it and corrupted the xrdp login field so it could not be typed
  into. All three call sites now substitute an empty string for a NULL username
  (`rdesktop/c/rdp`, `rdesktop/c/licence`).
- **Mouse pointer surrounded by a black square after login.** An
  operator-precedence bug in `Pointer_PrepMaskData` (`c/Pointer`) —
  `0x1 & source == 0` parses as `0x1 & (source == 0)` — inverted the AND-mask
  test, so the cursor's transparent surround was drawn opaque. Parenthesised to
  `(0x1 & source) == 0`. (xrdp users should also set `new_cursors=false` in
  `xrdp.ini` and `Xcursor.core: 1` in `~/.Xresources` for correct server-side
  cursors.) This corrects **standard-size** cursors; very large / high-resolution cursors (used by some Linux desktops) may still render imperfectly because RISC OS hardware pointers have size limits — a separate matter from the mask bug.
- **Clipboard paste from remote didn't work / produced mostly NULLs.** Fixed
  end-to-end in `c/Clipboard`: (1) **ownership** - when the Clipboard Store
  re-claims the entity just after we claim it, `actually_have_clipboard` was
  cleared *before* the "ignore the first holder re-claim" check, so RDPClient
  stopped answering paste requests; the unconditional clear is removed. (2)
  **negotiation** - servers such as **xrdp** and **NuoRDS** advertise only
  `CF_UNICODETEXT`, so `request_clipboard_data_from_server` now requests it when
  `CF_TEXT` is absent. (3) **data** - UTF-16LE text (with or without a BOM) is
  collapsed to Latin-1 (characters outside Latin-1 become `?`). (4) **all display
  modes** - the clipboard's single-task guards are lifted, so paste also works in
  full-screen mode. Full Unicode is still to come (see below).

### Known issues / next (→ 0.91)

- **Clipboard and usernames are Latin-1 only.** Full Unicode (correct handling
  of non-Latin-1 text in both directions) needs the RISC OS 5 **Iconv** module
  linked (`HAVE_ICONV` + libiconv); the `#ifdef HAVE_ICONV` code paths already
  exist in `rdesktop/c/rdp`. Deferred to **0.91**.
- Scroll wheel needs a real wheel device. Under **RPCEmu** the emulated machine
  must be set to 256MB RAM on RISC OS 5.30/5.31 for the wheel to be delivered.

### Licence

- 0.90.2 fixes touch `rdesktop/c/rdp`, `rdesktop/c/licence`, `c/Pointer` and
  `c/Clipboard`; each carries a **(C) 2026 Andrew Youll** modification notice.

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
