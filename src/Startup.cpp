#include "Startup.h"
#include <Directory.h>
#include <FindDirectory.h>
#include <Path.h>
#include <String.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>

namespace clipper {
static status_t LaunchPath(BPath& path, const char* directory) {
    if (directory) return path.SetTo(directory);
    status_t result = find_directory(B_USER_SETTINGS_DIRECTORY, &path);
    return result == B_OK ? path.Append("boot/launch") : result;
}

bool StartupEnabled(const char* launchDirectory) {
    BPath path;
    if (LaunchPath(path, launchDirectory) != B_OK || path.Append("Clipper") != B_OK)
        return false;
    struct stat info;
    return lstat(path.Path(), &info) == 0 && S_ISLNK(info.st_mode);
}

status_t SetStartupEnabled(bool enabled, const char* executable, const char* launchDirectory) {
    BPath directory;
    status_t result = LaunchPath(directory, launchDirectory);
    if (result != B_OK) return result;
    BPath link(directory.Path(), "Clipper");
    if (link.InitCheck() != B_OK) return link.InitCheck();
    struct stat info;
    bool exists = lstat(link.Path(), &info) == 0;
    if (!exists && errno != ENOENT) return errno;
    // Never overwrite or delete a user's regular file or directory.
    if (exists && !S_ISLNK(info.st_mode)) return B_FILE_EXISTS;
    if (!enabled) return !exists || unlink(link.Path()) == 0 ? B_OK : errno;
    if (!executable || executable[0] != '/') return B_BAD_VALUE;
    if (access(executable, X_OK) != 0) return errno;
    if (exists) {
        char target[B_PATH_NAME_LENGTH];
        ssize_t size = readlink(link.Path(), target, sizeof(target) - 1);
        if (size < 0) return errno;
        target[size] = '\0';
        if (BString(target) == executable) return B_OK;
    }
    result = create_directory(directory.Path(), 0755);
    if (result != B_OK) return result;
    // Stage beside launch/, so startup never sees a partially written entry.
    BPath parent;
    if ((result = directory.GetParent(&parent)) != B_OK) return result;
    BString temporary(parent.Path()); temporary << "/.clipper-startup-XXXXXX";
    char* tempDirectory = mkdtemp(temporary.LockBuffer(temporary.Length()));
    temporary.UnlockBuffer();
    if (!tempDirectory) return errno;
    BPath staged(temporary.String(), "Clipper");
    if (symlink(executable, staged.Path()) != 0) result = errno;
    else if (rename(staged.Path(), link.Path()) != 0) result = errno;
    unlink(staged.Path());
    rmdir(temporary.String());
    return result;
}
}
