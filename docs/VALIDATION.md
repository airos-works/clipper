# Laptop validation — 2026-10-20

Issues #1 and #2 were checked on the laptop through its MCP server, running
x86_64 air/OS/Haiku hrev60207+1716. Linux cross-builds completed without compiler
warnings. The laptop's native compiler could not execute its compiler frontend,
so the cross-built executables were uploaded and run natively there. Version
1.0.0-3 was packaged on the laptop and installed over 1.0.0-2 through MCP.

Verified:

- History, startup, and UI tests pass with zero failures. Startup tests cover
  repeated saves, existing/stale links, moved/missing executables, file/directory
  conflicts, and cleanup in an isolated temporary directory.
- The installed Settings checkbox starts from the actual login link. Save
  creates a link to `/boot/system/apps/Clipper`; reopening shows it checked.
  Unchecking and cancelling retains the link. Launching through that link after
  quitting starts Clipper with both history windows hidden. Unchecking and saving
  removes the link, and reopening shows it unchecked.
- The production shortcut filter's standalone test host passes with Clipper
  running and stopped: a tap returns one event pair; a stale timer does not open
  history; holding Alt+V and Shift+Alt+V open only the compact picker; launching
  from stopped works; repeated key-downs and injected paste behave correctly.
- Both compact and full-window paste integration tests pass for plain text plus
  HTML and byte-identical bitmap data. The tests leave focus in Clipper and
  verify that the app returns to the original native target before injection.
- MCP screenshots confirm the compact picker and startup checkbox render
  correctly. The picker has search, thumbnails, Copy/Paste, and no title tab or
  management/preview panels. UI tests cover dismissal, reopening, search, focus,
  and placement geometry on unequal, negative-origin, stacked and small screens.
- input_server has one packaged Clipper shortcut filter and one paste device.
  The original clipboard is restored by the integration tests; original history
  and preferences were restored afterward. Login startup remains disabled, as it
  was before testing. Clipper is left running in the background.

The shortcut test hosts the filter rather than injecting hardware keyboard
events. Startup was checked by launching the login link without rebooting the
laptop. Image acceptance by other applications remains destination-dependent.

# Workstation validation — 2026-10-02

Built and tested on 192.168.1.244, x86_64 Haiku hrev60097+178 with the air/OS
workstation build. Both the Linux cross-build and native Haiku build completed
without compiler warnings. The native package was created and its contents
verified, including the executable, both uniquely named input_server add-ons,
application menu link, MIME resources, and documentation.

Verified:

- History unit tests: deduplication, stable IDs, pins, eviction, fuzzy search,
  text/HTML preservation, bitmap byte roundtrip, atomic save/load, corrupt data,
  empty clipboard, and oversize clips.
- VNC copy and cut from a normal native text view populate history automatically.
- VNC tap Alt+V with the installed filter yields exactly one ordinary paste.
- Hold Alt+V opens history; search and Enter paste an older clip back into the
  original native window. Shift+Alt+V is also recognized.
- Native paste integration sends both text/HTML and images through the registered
  paste device into the original target. Restored bitmap bytes match the source.
  Sequential injections leave modifier state available for the next paste.
- Pin retention across a graceful quit/restart, pause/resume, saved preferences,
  clear unpinned while keeping pins, deletion, and persistence disabled across
  restart.
- Deskbar replicant registration and rendering, history and image preview layout,
  startup link, and one loaded shortcut filter plus one loaded paste device.

The earlier implementation was stopped and removed. Test history was cleared
and default preferences restored after verification. Clipper is left running in
its normal background mode with login startup enabled.

Image display/paste remains destination-dependent: the integration view proves
that image data reaches the original destination through the same keyboard paste
path; applications must support the supplied clipboard format to display it.
