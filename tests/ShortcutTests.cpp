// Exercise the production filter's timer/dispatcher against the installed app.
#include "Protocol.h"
#include <Application.h>
#include <InputServerFilter.h>
#include <InterfaceDefs.h>
#include <List.h>
#include <Message.h>
#include <Messenger.h>
#include <Roster.h>
#include <WindowInfo.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace clipper;
extern "C" BInputServerFilter* instantiate_input_filter();
// input_server exports these base-class symbols to its add-ons. Provide only
// the inert host scaffolding here; all shortcut behavior is the real filter.
BInputServerFilter::BInputServerFilter() {}
BInputServerFilter::~BInputServerFilter() {}
status_t BInputServerFilter::InitCheck() { return B_OK; }
filter_result BInputServerFilter::Filter(BMessage*,BList*) { return B_DISPATCH_MESSAGE; }
status_t BInputServerFilter::GetScreenRegion(BRegion*) const { return B_NOT_SUPPORTED; }
void BInputServerFilter::_ReservedInputServerFilter1() {}
void BInputServerFilter::_ReservedInputServerFilter2() {}
void BInputServerFilter::_ReservedInputServerFilter3() {}
void BInputServerFilter::_ReservedInputServerFilter4() {}
static int failures=0;
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c); ++failures; } } while(0)

static bool Visible(const char* title) {
    app_info app;
    if(be_roster->GetAppInfo(kSignature,&app)!=B_OK) return false;
    int32 count=0; int32* tokens=get_token_list(app.team,&count); bool visible=false;
    for(int32 i=0;i<count;++i) {
        auto info=get_window_info(tokens[i]);
        if(info && strcmp(info->name,title)==0 && info->show_hide_level==0) visible=true;
        free(info);
    }
    free(tokens); return visible;
}
static bool WaitForPopup() {
    for(int i=0;i<40;++i) { if(Visible("Clipper popup")) return true; snooze(50000); }
    return false;
}
static void Hide() {
    BMessage hide(kHide); BMessenger(kSignature).SendMessage(&hide);
    snooze(100000);
}
int main() {
    BApplication app("application/x-vnd.airOS-Clipper-ShortcutTests");
    auto filter=instantiate_input_filter(); CHECK(filter->InitCheck()==B_OK);
    Hide();
    BMessage down(B_KEY_DOWN); down.AddInt32("key",0x56); down.AddInt32("raw_char",'v');
    down.AddInt32("modifiers",B_COMMAND_KEY); down.AddString("bytes","v"); down.AddInt64("when",system_time());
    BMessage up(down); up.what=B_KEY_UP;
    BList events;
    CHECK(filter->Filter(&down,&events)==B_SKIP_MESSAGE);
    CHECK(filter->Filter(&up,&events)==B_DISPATCH_MESSAGE);
    CHECK(events.CountItems()==2);
    if(events.CountItems()==2) {
        CHECK(static_cast<BMessage*>(events.ItemAt(0))->what==B_KEY_DOWN);
        CHECK(static_cast<BMessage*>(events.ItemAt(1))->what==B_KEY_UP);
    }
    for(int32 i=0;i<events.CountItems();++i) delete static_cast<BMessage*>(events.ItemAt(i));
    events.MakeEmpty(); snooze(500000); CHECK(!Visible("Clipper popup")); // stale timer

    CHECK(filter->Filter(&down,&events)==B_SKIP_MESSAGE);
    CHECK(filter->Filter(&down,&events)==B_SKIP_MESSAGE); // repeat
    CHECK(WaitForPopup());
    CHECK(!Visible("Clipper"));
    CHECK(filter->Filter(&up,&events)==B_SKIP_MESSAGE);
    CHECK(events.IsEmpty());
    Hide();
    down.ReplaceInt32("modifiers",B_COMMAND_KEY|B_SHIFT_KEY);
    CHECK(filter->Filter(&down,&events)==B_SKIP_MESSAGE);
    CHECK(WaitForPopup());
    CHECK(!Visible("Clipper"));
    CHECK(filter->Filter(&up,&events)==B_SKIP_MESSAGE);
    CHECK(events.IsEmpty());
    Hide();
    down.AddBool("clipper:injected",true);
    CHECK(filter->Filter(&down,&events)==B_DISPATCH_MESSAGE);
    delete filter;
    printf("Shortcut tests: %d failures\n",failures);
    return failures?1:0;
}
