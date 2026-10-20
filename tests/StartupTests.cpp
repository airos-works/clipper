#include "Startup.h"
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <Path.h>
#include <String.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
using namespace clipper;
static int failures = 0;
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c); ++failures; } } while (0)

static bool PointsTo(const char* link, const char* target) {
    char buffer[B_PATH_NAME_LENGTH];
    ssize_t size=readlink(link,buffer,sizeof(buffer)-1);
    if(size<0) return false;
    buffer[size]='\0';
    return strcmp(buffer,target)==0;
}

int main() {
    char root[]="/tmp/clipper-startup-tests-XXXXXX";
    if(!mkdtemp(root)) return 1;
    BPath launch(root,"boot/launch"), link(launch.Path(),"Clipper");
    BPath app(root,"Clipper"), moved(root,"Moved Clipper");
    { BFile file(app.Path(),B_WRITE_ONLY|B_CREATE_FILE); CHECK(file.InitCheck()==B_OK); }
    CHECK(chmod(app.Path(),0755)==0);
    CHECK(!StartupEnabled(launch.Path()));
    CHECK(SetStartupEnabled(false,nullptr,launch.Path())==B_OK);
    CHECK(!BEntry(launch.Path()).Exists()); // disabling creates nothing
    CHECK(SetStartupEnabled(true,app.Path(),launch.Path())==B_OK);
    CHECK(StartupEnabled(launch.Path()));
    CHECK(PointsTo(link.Path(),app.Path()));
    CHECK(SetStartupEnabled(true,app.Path(),launch.Path())==B_OK); // repeated save
    CHECK(rename(app.Path(),moved.Path())==0);
    CHECK(StartupEnabled(launch.Path())); // preserve a prior choice after a move
    CHECK(SetStartupEnabled(true,moved.Path(),launch.Path())==B_OK);
    CHECK(PointsTo(link.Path(),moved.Path()));
    CHECK(SetStartupEnabled(true,app.Path(),launch.Path())!=B_OK); // missing target
    CHECK(PointsTo(link.Path(),moved.Path())); // retain working link on failure
    CHECK(SetStartupEnabled(false,nullptr,launch.Path())==B_OK);
    CHECK(!StartupEnabled(launch.Path()));
    CHECK(SetStartupEnabled(false,nullptr,launch.Path())==B_OK);
    // Recognize and disable a legacy/manual relative link, including a stale one.
    CHECK(symlink("../../Clipper",link.Path())==0);
    CHECK(StartupEnabled(launch.Path()));
    CHECK(SetStartupEnabled(false,nullptr,launch.Path())==B_OK);
    { BFile file(link.Path(),B_WRITE_ONLY|B_CREATE_FILE); file.Write("keep",4); }
    CHECK(!StartupEnabled(launch.Path()));
    CHECK(SetStartupEnabled(true,moved.Path(),launch.Path())==B_FILE_EXISTS);
    CHECK(SetStartupEnabled(false,nullptr,launch.Path())==B_FILE_EXISTS);
    { BFile file(link.Path(),B_READ_ONLY); char value[4]; CHECK(file.Read(value,4)==4); CHECK(memcmp(value,"keep",4)==0); }
    CHECK(unlink(link.Path())==0);
    CHECK(mkdir(link.Path(),0755)==0);
    CHECK(SetStartupEnabled(false,nullptr,launch.Path())==B_FILE_EXISTS);
    CHECK(rmdir(link.Path())==0);
    CHECK(SetStartupEnabled(true,"relative/path",launch.Path())==B_BAD_VALUE);
    CHECK(unlink(moved.Path())==0);
    CHECK(rmdir(launch.Path())==0);
    BPath boot(root,"boot"); CHECK(rmdir(boot.Path())==0);
    CHECK(rmdir(root)==0); // no staging files left behind
    printf("Startup tests: %d failures\n",failures);
    return failures?1:0;
}
