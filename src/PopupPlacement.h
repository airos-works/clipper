#pragma once
#include <Rect.h>
#include <Size.h>

namespace clipper {
BRect PopupScreenFrame(BPoint cursor);
BRect PopupFrame(BPoint cursor, BSize size, BRect screen);
}
