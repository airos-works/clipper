# Clipper

A native clipboard manager for **air/OS and Haiku**, built with the Haiku kits.
Runs quietly in the Deskbar and captures clipboard changes from any application,
including menu copy, **Alt+C**, **Alt+X**, and screenshots.

<!-- airos-ci:latest-builds:start -->
## Latest builds

Built automatically by air/OS CI from commit `2d0e0cf` on 2026-10-20 ([all files](https://github.com/jmgasper/clipper/releases/tag/latest)).

| Architecture | Package |
|---|---|
| arm64 | [clipper-1.0.0-3-arm64.hpkg](https://github.com/jmgasper/clipper/releases/download/latest/clipper-1.0.0-3-arm64.hpkg) |
| x86_64 | [clipper-1.0.0-3-x86_64.hpkg](https://github.com/jmgasper/clipper/releases/download/latest/clipper-1.0.0-3-x86_64.hpkg) |

Install with `pkgman install <file>`, or copy the file into `/boot/system/packages`. Haiku SDK: x86_64 hrev60207-1733-g3e7f3a491b, arm64 hrev60207-1733-g3e7f3a491b.
<!-- airos-ci:latest-builds:end -->

## Using Clipper

- **Tap Alt+V**: paste the current clipboard, normally the last item copied or cut.
- **Hold Alt+V for 350 ms**, or **Shift+Alt+V**: open a compact, searchable popup
  beside the cursor. It stays within that monitor; **Esc**, clicking outside, or
  switching windows dismisses it without changing the clipboard.
- Type to search; use **Up/Down** to select, then **Enter** to paste into the
  window you came from. Double-clicking a row or clicking **Paste** does the same.
- **Copy** restores the selected item without pasting it. **Esc** hides history.
- Click the **Deskbar clipboard icon** to open history; right-click for settings,
  pause/resume recording, or quit.
- **Pin** keeps reusable clips above the others and protects them from automatic
  eviction. **Delete** removes a selected clip. **Clear…** can keep pins or erase all.

Text, rich text, native Haiku bitmaps, encoded images, and file references retain
all clipboard formats. Image rows have thumbnails and a larger preview; text rows
have a readable preview. Fuzzy search covers text, source application, and type.
The interface follows Haiku's system colors and fonts.

**Settings** includes **Start Clipper when the computer starts**. Enable it and
click **Save** to launch Clipper in the background at login; uncheck it to stop
automatic startup. Existing startup links are recognized. Cancel leaves the
startup choice unchanged, and updates preserve your choice.

Settings also controls the history limit (10–1000, default 100), persistence, and
paste on selection. The explicit **Paste** button always pastes. History is local under `~/config/settings/Clipper/`, saved
atomically with owner-only permissions. Pause temporarily disables recording.
Each clip can be up to **32 MiB**; total history is limited to **128 MiB**. Larger
clips still paste normally, but are not stored. Pins are retained if the item limit
is reduced below the number of existing pins. Pins never allow new capture to
exceed the total byte limit.

Automatic paste uses a native input device, returns to the original window, waits
for physical modifiers to be released, and injects one ordinary paste shortcut.
If focus changes or the device is unavailable, the item remains copied for manual
paste. Image paste requires a destination application that accepts images.

## Build and install on Haiku

```sh
make -j8
make check
bash tools/install.sh
```

This installs `~/config/non-packaged/apps/Clipper` and the two input_server add-ons.
Enable login startup in Settings. Existing Clipper history and startup choices
are preserved during updates.
No input_server restart is needed; allow a few seconds for add-on activation.

```sh
bash tools/install.sh --uninstall  # retains saved history
make package                      # artifacts/clipper-1.0.0-3-x86_64.hpkg
```

For a packaged install, remove the development install first, then use
`pkgman install <package>`. Launch Clipper from Applications and enable login
startup in Settings if desired.
Do not keep packaged and non-packaged input add-ons installed together.

The application starts in the background; `Clipper --history` opens the full
history window, `Clipper --popup` opens the cursor popup, and `Clipper --quit`
exits the running instance.

## Development

`tools/build-cross.sh` uses the local x399 Haiku build tree to cross-compile on
Linux. `tools/deploy.sh` uses SSH to sync, build, test, install, and package on
192.168.1.244. Override `CLIPPER_WS_HOST`, `CLIPPER_WS_USER`, or `CLIPPER_WS_KEY`
for another workstation. No account passwords are included in this repository.

To package cross-built binaries on Haiku without compiling there, copy
`build-cross/Clipper`, `Clipper_filter`, `Clipper_device`, and `Clipper.rsrc`
alongside the source tree, then run
`BUILD=build-cross bash tools/package-haiku.sh --prebuilt`.

See [design](docs/DESIGN.md) and [testing](docs/TESTING.md).

Inspired by [CopyClip](https://github.com/Walkercito/CopyClip); this implementation
is original native Haiku C++ and does not depend on GTK, Qt, X11, or Wayland.
The clipboard icon is original vector artwork with a native HVIF resource.

MIT license; see [LICENSE](LICENSE).
