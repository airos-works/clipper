# Testing

Run on Haiku:

```sh
make -j8 all check check-ui build-haiku/fixture build-haiku/paste_integration
make build-haiku/shortcut_tests
```

`ui_tests` checks search arrow navigation, Enter on a focused Settings button,
reopening after repeated dismissal, explicit pause/resume labels, a one-pixel-wide
image preview, and the settings Cancel button and initial focus. Popup tests
cover placement on unequal, negative-origin, stacked, and small monitor frames;
search, selection, and initial focus; repeated Esc/outside-click/focus/workspace
dismissal; listener cleanup; relocation on reopening; and empty history. It runs
against an in-memory history and does not read or change saved clips.

`history_tests` checks stable-ID deduplication, newest-first ordering, pin
protection, count eviction, fuzzy search, rich-text preservation, byte-identical
native bitmap roundtrip through persistence, invalid archive rejection, empty
clipboard rejection, and maximum clip size. Temporary archives live under `/tmp`.

`startup_tests` uses a temporary directory to verify enabling/disabling login
startup, repeated saves, existing and stale links, moved executables, invalid
targets, preservation of conflicting files/directories, and staging cleanup.
It never changes the user's login startup. The UI test checks that the startup
checkbox reflects the current login link.

`shortcut_tests` hosts the production filter with an inert input_server base
class scaffold, and sends its real timer/dispatcher requests to the installed
Clipper. It checks a single tap's event pair, stale timers, repeat suppression,
long-press and Shift+Alt+V opening only the compact picker, and injected-paste
bypass. Run with Clipper both running and stopped to cover launch-on-demand.
This does not synthesize hardware input through the installed input_server.

`build-haiku/paste_integration` opens a temporary native paste target, captures
rich text and a bitmap, opens Clipper, selects each captured item, and verifies
that the injected paste reaches the original target with every format intact.
The bitmap payload is compared byte for byte. The test also exercises sequential
paste injections, including restoration of modifier state. Pass `--popup` to
exercise the compact picker; run both modes. Focus stays in Clipper until the
app restores the original target. The test restores the initial clipboard on
exit. It requires persistence and recording enabled; back up history before
running because its test clips are captured normally.

The native fixture provides a standard editable text view and overrides its
normal clipboard paste method to archive every payload under
`/tmp/clipper-last-paste`. This checks real input_server-injected shortcuts,
including image formats that an ordinary text view does not display.

```sh
build-haiku/fixture
build-haiku/fixture --set-text 'sample'
build-haiku/fixture --set-image path/to/image.png
build-haiku/fixture --history
build-haiku/fixture --read-paste
build-haiku/fixture --deskbar
build-haiku/fixture --hide
```

For VNC tests, connect to 192.168.1.244:5900 using the workstation credentials.
Keyboard events from its InputEventInjector pass through input_server's filters.
The NanoKVM is an alternate USB keyboard path. Avoid concurrent input from other
clients. Full-frame captures are more reliable than partial captures while
Summit is running a direct-rendered graphics/video benchmark.

Set `CLIPPER_TRACE=1` when launching to log show requests, chosen IDs, destination
team/window, and paste bridge results. This does not log clipboard text.

Manual coverage:

1. Select text in the fixture and use Alt+C. Confirm the history's text and source.
2. Select different text and use Alt+X. Confirm the source text was cut and recorded.
3. Tap Alt+V. Confirm exactly one ordinary paste of the current clip.
4. Hold Alt+V at least 350 ms. Release V and Alt; the compact popup opens beside
   the cursor without pasting. Repeat on each monitor, near all four edges and
   the seam; the entire popup should stay on the cursor's monitor. Move the cursor
   elsewhere and reopen to confirm that the old position is not reused.
5. Search an older clip, navigate with arrows, and Enter. Confirm the fixture's
   pasted payload matches that older clip and history closes.
6. Shift+Alt+V opens the same popup immediately. Escape closes; no paste occurs.
   Reopen and click another window, the desktop, or Deskbar: the popup closes and
   the click still reaches that surface. Switch windows/workspaces to dismiss.
   Repeat dismissal/reopening and verify that the clipboard remains unchanged.
7. Copy an image. Confirm dimensions, thumbnail, preview, and restored bitmap data
   in `--read-paste`. Also paste into an image-capable application.
8. Pin a clip; recopy it and restart. Confirm its ID stays stable and pin survives.
9. Delete a clip, clear unpinned history, and confirm the pin remains.
10. Pause recording, copy a unique string, resume, and confirm it was not recorded.
11. Disable persistence, quit/restart, and confirm saved history is removed.
12. Open Settings, change the limit and paste preference, save, and restart.
    Toggle **Start Clipper when the computer starts**, save, and reopen Settings.
    Confirm `~/config/settings/boot/launch/Clipper` points to the running executable
    when enabled and is absent when disabled. Cancel must leave it unchanged.
    Quit and launch using that link to verify background startup. An app update
    must preserve both enabled and disabled choices.
13. Switch focus during paste-back. The app should leave the clip copied and cancel
    injection, rather than paste into a different application.
14. Check the Deskbar icon, its menu, startup link, and absence of duplicate add-ons.
15. Open full history from Deskbar, then invoke the popup; only the popup should
    remain visible. Return through Deskbar and verify full history still offers
    previews, pins, deletion, and settings. With Clipper stopped, invoke the
    shortcut and confirm it launches directly into the popup.

The workstation's previous implementation was removed before installing this
replacement: its executable, boot script, lowercase settings, MIME registration,
Deskbar replicant, running team, and `/boot/home/clipper` source tree. The latter
path now contains the new Clipper source and build.
