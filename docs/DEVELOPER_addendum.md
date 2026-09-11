

---

# Runtime bugs found AFTER the build succeeded (Norcroft / 32-bit / RISC OS 5)

Getting a clean compile+link is only half the job. The 32-bit Norcroft build
launched and connected but showed a **blank/black screen**, then (once that was
fixed) a chain of further problems. These are all compiler- or platform-specific
— they do not reproduce in the original 26-bit binary — so they are documented
here in the order we hit them, with the diagnosis method, because the next person
to touch this port will almost certainly meet the same class of bug.

Golden rule learned repeatedly: **when the source looks obviously correct but the
runtime behaviour contradicts it, suspect Norcroft codegen / struct layout /
alignment before anything else, and confirm by logging actual values at
runtime** (via `error()` / `warning()`, which land in the app's Log window).

## A. Struct layout: `unsigned int : 1` bitfields (fatal on launch)

Symptom: `RDPClient has suffered a fatal internal error (not enough memory to
copy template)` immediately on run; never reached the icon bar.

Cause: DeskLib's `wimp_colourflags` (in `DeskLib:Wimp.h`) declared several
`unsigned int : 1` bitfields directly after `char` fields. Norcroft aligns the
storage unit for `unsigned int` bitfields to a word, so the struct came out
**12 bytes instead of 8**. That shifted `window_block.numicons` to the wrong
offset — `Template_Clone` read `numicons` as 24 (garbage) instead of 4 and tried
to `malloc` a wildly oversized template.

Diagnosis: added `sprintf` diagnostics into the four malloc-fail paths of
`Libraries/Template/Clone.c` and a `sizeof`/offset probe; saw `numicons=24`,
`sizeof(window_block)` wrong.

Fix: replace the bitfield group with a single `unsigned char extflags;` (nothing
references the individual flag bits). `char` bitfields are not an option —
ANSI C / Norcroft rejects `char` bit-field types.

Lesson: **never trust `unsigned int : N` bitfield layout under Norcroft** for
structs that must match a binary/OS layout. Use explicit `unsigned char` fields.

## B. Stream parsing: unaligned + wrong-endian macro path (blank screen)

Symptom: connects, window opens at the correct remote size, cursor blanks over
the window, **but nothing ever draws** (transparent window in windowed mode,
black in full screen). Log empty.

Method that cracked it: instrument the whole receive path with `error()` probes
(`c/UI ui_paint_bitmap`, `c/Display redraw_loop`, then `rdesktop/c/rdp`
`rdp_loop`/`process_data_pdu`/`process_update_pdu`, then `rdesktop/c/secure`
`sec_recv`, then `rdesktop/c/iso` `iso_recv_msg`, then `rdesktop/c/mcs`
`mcs_recv`). Each build recompiles only the touched objects (see "chunked build"
above), so a probe cycle is a couple of minutes, not the full ~40.

Finding: the MCS channel id was read as e.g. `23776`/`23904`/`24004` (varying per
build) instead of `1003`. A raw byte dump proved the bytes on the wire were
`… 3 235 …` = `0x03EB` = 1003 — correct data, wrong read.

Root cause: `rdesktop/h/parse` chooses between a "fast" path
(`v = *(uint16*)((s)->p)`) and a byte-by-byte path via
`#if defined(L_ENDIAN) && !defined(NEED_ALIGN)`. The fast path does **unaligned**
16/32-bit loads (rotated garbage on ARM at odd offsets) and its `_be` variant is
only correct on a big-endian CPU. `NEED_ALIGN` was meant to be auto-defined for
non-x86 but was not taking effect at macro-selection time.

Fix: stop relying on the conditional entirely — `parse.h` now defines the
`in_/out_uint16/32_le/be` macros **unconditionally, byte-by-byte and
endian-correct** (little-endian native). This is alignment-safe and correct
regardless of which endian/align symbols happen to be defined.

Lesson: on RISC OS/ARM, **all multi-byte protocol reads/writes must be
byte-by-byte**; never `*(uintNN*)ptr` on packet buffers.

## C. Norcroft codegen: output parameter / local not written back (blank screen, part 2)

Symptom: after fix B, `mcs_recv()` computed the channel correctly (a probe
inside it printed `macro=1003 manual=1003`), but back in `sec_recv()` the local
`channel` — filled via `mcs_recv(&channel, …)` — read as garbage that *changed
between builds* (24088, 24124, …). Even inserting `channel = <known good>;`
right before use did not stick. Reading a mirror global in `sec_recv` returned
values that tracked an unrelated static counter — i.e. the optimiser was
aliasing/caching reads of that variable in this function.

So: `mcs_recv()` parses the channel correctly, but the compiler fails to deliver
it to `sec_recv()` (both via the `uint16 *` out-param and via a plain local
assignment) inside that `while` loop. Other globals read in the same function
(`g_licence_issued`, `g_encryption`, both `int`) work fine — the mangled ones
were `uint16`.

Fix (robust): `mcs_recv()` stores the parsed channel in a file-scope
**`volatile int g_diag_mcs_chan`**; `sec_recv()` uses `g_diag_mcs_chan` directly
in `if (… != MCS_GLOBAL_CHANNEL)` and `channel_process(…)`, ignoring the local.
`volatile` forces a genuine memory load each time (defeats the caching/aliasing);
`int` (32-bit) avoids the miscompiled 16-bit load path from lesson B.

Lesson: if a value that is provably correct at one point reads as garbage a few
lines later under Norcroft, suspect **register caching / aliasing of a `short`
local**. Escalate to `volatile int` in file scope and read it directly.

After A+B+C the session connects and the desktop draws.

## D. (open) Scroll wheel — how RISC OS 5 actually delivers it

**The flag was wrong at first.** The original `window_flag_SCROLLREQUEST = 0x10`
is **bit 4 = auto-redraw**, not scroll. The correct request bits are **bit 8**
("Return Scroll_Request with auto-repeat") or **bit 9** (without auto-repeat) of
the window-flags word. We now set `flags.data.scrollrq = 1` (bit 8) via the
DeskLib bitfield.

**Two flags are needed for extended (wheel) requests.** Per the RISC OS Window
Manager docs and the riscosopen.org forum (topic 17041), the mouse wheel reaches
an app as a **reason-10 Scroll_Request** only when the window has **both**:
1. a scroll-request bit set (bit 8 or 9 of the window-flags word at +28), and
2. **bit 1 of the extra-flags byte at offset +39** ("extended scroll request").
Without (2), the WindowScroll module auto-scrolls the window itself instead of
telling the app (André Timmermans, same thread). We set (2) via
`colours.cols.extflags |= 0x02`.

Scroll_Request block layout (reason 10), confirmed against DeskLib's `scroll_rq`
(which places `direction` at exactly +32/+36):

    +0  window handle          +20 scroll x offset     +36 scroll y DIRECTION
    +4..+16 visible area        +24 scroll y offset     +40 dest icon (ext only)
                                +28 window behind
                                +32 scroll x DIRECTION

Direction values (Steve Fryatt, same thread): `+/-1` line, `+/-2` page, `+/-3`
autoscroll, and for **extended** requests the value arrives as a multiple of
`+/-4` where `value >> 2` = the number of wheel notches. `Mouse_Wheel()` in
`c/Mouse` decodes all of these. Extended requests need **RISC OS 5.27+**.

**Both delivery paths are now handled** so the wheel works whichever way this
hardware sends it:
- `ScrollRequest()` (reason 10) → `Mouse_Wheel()`.
- `OpenWindow()` intercept: if the requested scroll offset changed (the
  WindowScroll module auto-scrolling us) → `Mouse_Wheel()` and pin the window
  back so the RDP display itself never scrolls.

**What the live probes established so far.** `Window_GetInfo` is a real
`Wimp_GetWindowInfo` SWI, so its readback reflects the actual Wimp window — and
it shows `flags=0x…102 scrollrq=1 extflags=0x2 extscroll(bit1)=1` on the RDP
window. So the Wimp *knows* we want (extended) scroll requests. In the one
captured test, the only wheel-related event was a reason-2 Open_Window to a
window handle that was **neither** `disp_win` nor `fs_win` — i.e. the **Log
window** (which the user hovers to save the log; it has no scroll flag, so the
system auto-scrolls it, exactly as expected). No event reached the RDP window in
that test. The poll probe now **labels** each OPEN/SCROLL with `(RDP/FS/other)`
so a single clean test — scrolling with the pointer over the RDP desktop — tells
us definitively which mechanism (if any) the RDP window receives.

Two known things that can suppress reason-10 on this class of machine (Steve
Fryatt, same thread): the **Mouse → scroll-wheel Configure** setting, and **HID**
mouse support, which "went to great lengths to present scroll wheel use as normal
scroll events" (i.e. auto-scroll) for apps that don't opt in. If reason 10 never
appears even over the RDP window, the robust fallback is the OpenWindow
auto-scroll intercept (already in place); its one weakness is that at a scroll
limit the Wimp generates no Open_Window, so the window must sit mid-extent to
catch both directions.

### The standalone capture test (`WheelTest`)

To prove what the wheel does **independently of this C app** (Norcroft, focus
logic, the RDP session), `WheelTest` is a ~30-line BBC BASIC WIMP program that
needs no build service — it runs natively on the Pi. It creates one window with
bit 8 + extra-flag bit 1 set and shows live in its title bar: `S10=` (reason-10
count, plus a beep on each) vs `Op=`/`sy=` (auto-scroll). If `S10` rises when you
wheel over it, reason-10 delivery works on this machine and any remaining problem
is in our app; if only `Op`/`sy` move, the machine is auto-scrolling and we rely
on the OpenWindow path. To run it: Ctrl-F12 for a Task window, type `BASIC`,
paste the numbered listing at the `>` prompt, type `RUN`; click the window's
close icon to quit.

(Earlier diagnostic probes remain: `ScrollRequest`, the `Key` handler — which
logs only key codes ≥ 0x180 so typed text / passwords are never logged — and the
session poll probe in `RDPClient_PollNull`.)

---

# Delivering the binary (avoid the stale-file trap)

`buildapp.py` writes the linked absolute to a file literally named **`RDPClient`**
(RISC OS filetype comes back as `&a91`; it must be set to **`&FF8`, Absolute**,
inside `!RDPClient`). On the Mac's HFS/FUSE view a file typed `&FF8` shows with a
`,ff8` suffix, so an *older* copy can sit next to the fresh `RDPClient` as
`RDPClient,ff8`. Copying the `,ff8` file then runs a **stale** binary — this cost
us several confusing test cycles.

Rule: after every build, refresh the deliverable so the file you copy to the Pi
is the one you just built, e.g. on the Mac:

    cd "…/build"
    cat RDPClient > "RDPClient,ff8"     # overwrite content, keep the &FF8 type

then copy `RDPClient,ff8` into `!RDPClient`. Or just copy the freshly-written
`RDPClient` and set its type to `&FF8`.

# Debugging technique that worked

- **Log to the app's Log window** with `error("DIAG …", …)` / `warning(...)`
  (both call `Log_AddMessage`). The Log is a capped ring that keeps the newest
  messages, and `error()` auto-opens the window — so a single decisive probe is
  visible without any RISC OS-side setup. Prefix everything `DIAG` so it is easy
  to grep/screenshot.
- **One decisive probe per cycle.** Each cycle is: edit source in `work/…`, move
  the affected `work/o/<obj>` aside so it recompiles, run `buildapp.py`, refresh
  `RDPClient,ff8`, run on the Pi, read the Log. Only the touched objects rebuild.
- **Dump raw bytes** when a parsed value looks wrong, to separate "bad data" from
  "bad read" — that is what proved the channel bug was a read bug, not a wire or
  offset problem.
- Confirm which binary actually ran: `strings "RDPClient,ff8" | grep 'DIAG <new probe>'`
  before trusting a "same as last time" result.
