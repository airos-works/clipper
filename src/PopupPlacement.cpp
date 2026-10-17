#include "PopupPlacement.h"
#include <Screen.h>
#include <algorithm>

namespace BPrivate {
// air/OS exposes physical monitor bounds separately from BScreen's combined
// desktop. A weak reference keeps the same binary usable on standard Haiku.
status_t get_display_frame(BRect frame, bool forZoom, BRect& displayFrame)
    __attribute__((weak));
}

namespace clipper {
BRect PopupScreenFrame(BPoint cursor) {
    BRect frame;
    if (BPrivate::get_display_frame != nullptr
        && BPrivate::get_display_frame(BRect(cursor, cursor), false, frame) == B_OK
        && frame.IsValid()) return frame;

    BScreen screen;
    frame = screen.Frame();
    if (screen.IsValid()) do {
        if (screen.Frame().Contains(cursor)) return screen.Frame();
    } while (screen.SetToNext() == B_OK);
    return frame;
}

BRect PopupFrame(BPoint cursor, BSize size, BRect screen) {
    // Leave room for the thin window border, and flip above/left of the cursor
    // at a monitor edge. Clamp against this monitor, including negative origins.
    BRect available = screen.InsetByCopy(8, 8);
    if (!available.IsValid()) available = screen;
    float width = std::min(size.Width(), available.Width());
    float height = std::min(size.Height(), available.Height());
    float x = cursor.x + 8, y = cursor.y + 8;
    if (x + width > available.right) x = cursor.x - width - 8;
    if (y + height > available.bottom) y = cursor.y - height - 8;
    x = std::max(available.left, std::min(x, available.right - width));
    y = std::max(available.top, std::min(y, available.bottom - height));
    return BRect(x, y, x + width, y + height);
}
}
