#pragma once
#include <SupportDefs.h>

namespace clipper {
// The login link is the preference, including links made by older installers.
// A directory override lets tests exercise this without changing login startup.
bool StartupEnabled(const char* launchDirectory = nullptr);
status_t SetStartupEnabled(bool enabled, const char* executable,
    const char* launchDirectory = nullptr);
}
