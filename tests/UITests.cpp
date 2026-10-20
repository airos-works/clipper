#include "HistoryWindow.h"
#include "PopupPlacement.h"
#include "Protocol.h"
#include "Startup.h"
#include <Application.h>
#include <Bitmap.h>
#include <Button.h>
#include <CheckBox.h>
#include <ListView.h>
#include <Layout.h>
#include <TextControl.h>
#include <TextView.h>
#include <atomic>
#include <cstdio>
#include <cstring>
using namespace clipper;
static int failures = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #c); ++failures; } } while (0)
static BPoint Center(BRect frame) { return BPoint(frame.left+frame.Width()/2,frame.top+frame.Height()/2); }
class TestWindow : public HistoryWindow {
public:
    std::atomic<bool> settingsInvoked{false};
    void MessageReceived(BMessage* message) override {
        if (message->what == kSettings) settingsInvoked = true;
        else HistoryWindow::MessageReceived(message);
    }
};
static void CheckPopupPlacement() {
    // Unequal side-by-side displays, a display left/above the primary, and a
    // small screen. No placement should span the seam or escape its monitor.
    for (BRect screen : {BRect(0,0,3839,2159), BRect(3840,0,6399,1439),
            BRect(-2560,-500,-1,939), BRect(0,-2160,3839,-1), BRect(0,0,319,239)}) {
        for (BPoint cursor : {screen.LeftTop(), screen.RightTop(), screen.LeftBottom(),
                screen.RightBottom(), Center(screen)}) {
            BRect frame = PopupFrame(cursor, BSize(480,420), screen);
            CHECK(frame.IsValid());
            CHECK(screen.Contains(frame));
            CHECK(frame.Width() <= 480 && frame.Height() <= 420);
        }
    }
    BRect normal = PopupFrame(BPoint(100,100), BSize(480,420), BRect(0,0,3839,2159));
    CHECK(normal.LeftTop() == BPoint(108,108));
    BRect edge = PopupFrame(BPoint(3839,2159), BSize(480,420), BRect(0,0,3839,2159));
    CHECK(edge.RightBottom() == BPoint(3831,2151));
}
static void CheckPopup(const History& history) {
    auto* popup = new HistoryWindow(true);
    popup->Run(); popup->Lock();
    popup->Update(history, false);
    BPoint cursor = Center(PopupScreenFrame(BPoint(100,100)));
    popup->OpenAt(cursor);
    popup->Unlock(); snooze(100000); popup->Lock();
    CHECK(!popup->IsHidden());
    CHECK(PopupScreenFrame(cursor).Contains(popup->Frame()));
    CHECK(popup->Look() == B_BORDERED_WINDOW_LOOK);
    auto* search = dynamic_cast<BTextControl*>(popup->FindView("search"));
    auto* list = dynamic_cast<BListView*>(popup->FindView("clips"));
    auto* paste = dynamic_cast<BButton*>(popup->FindView("paste"));
    CHECK(search && list && paste);
    CHECK(popup->FindView("settings") == nullptr);
    CHECK(popup->FindView("preview deck") == nullptr);
    CHECK((popup->GetLayout()->Owner()->EventMask() & B_POINTER_EVENTS) != 0);
    if (search && list && paste) {
        CHECK(popup->CurrentFocus() == search->TextView());
        CHECK(list->CountItems() == int32(history.clips.size()));
        BMessage down(B_KEY_DOWN); char arrow[] = {B_DOWN_ARROW,0}; down.AddString("bytes",arrow);
        popup->DispatchMessage(&down,search->TextView());
        CHECK(list->CurrentSelection() == 1);
        search->SetText("no such clipboard item");
        BMessage searchChanged(kSearch); popup->MessageReceived(&searchChanged);
        CHECK(list->CountItems() == 0);
        CHECK(!paste->IsEnabled());

        BMessage escape(B_KEY_DOWN); char esc[] = {B_ESCAPE,0}; escape.AddString("bytes",esc);
        popup->DispatchMessage(&escape,search->TextView());
        CHECK(popup->IsHidden());
        CHECK(popup->GetLayout()->Owner()->EventMask() == 0);
        popup->Dismiss(); popup->OpenAt(cursor);
        CHECK(!popup->IsHidden());
        CHECK(strcmp(search->Text(), "") == 0);
        CHECK(list->CountItems() == int32(history.clips.size()));
        CHECK(list->CurrentSelection() == 0);
        // A pinned row before the latest clip must not be partially scrolled
        // offscreen when both rows fit in the freshly laid out popup.
        History pinned = history; pinned.clips.back().pinned = true;
        popup->Update(pinned, false); popup->OpenAt(cursor);
        CHECK(list->CurrentSelection() == 1);
        CHECK(list->Bounds().top == 0);

        // An inside click keeps the popup; an outside click closes it even
        // without a WindowActivated(false) notification.
        BMessage click(B_MOUSE_DOWN); click.AddPoint("screen_where",Center(popup->Frame()));
        popup->DispatchMessage(&click,popup->GetLayout()->Owner());
        CHECK(!popup->IsHidden());
        click.ReplacePoint("screen_where",popup->Frame().LeftTop()-BPoint(20,20));
        popup->DispatchMessage(&click,popup->GetLayout()->Owner());
        CHECK(popup->IsHidden());
        popup->OpenAt(cursor);
        popup->WindowActivated(false);
        CHECK(popup->IsHidden());
        popup->OpenAt(cursor);
        popup->WorkspaceActivated(current_workspace(),false);
        CHECK(popup->IsHidden());

        // Each invocation relocates, independent of the previous popup frame.
        BPoint next = PopupScreenFrame(cursor).RightBottom();
        popup->OpenAt(next);
        CHECK(PopupScreenFrame(next).Contains(popup->Frame()));
        CHECK(popup->Frame().RightBottom().x <= next.x);
        popup->Unlock(); snooze(100000); popup->Lock();
        CHECK(!popup->IsHidden());
        History empty; popup->Update(empty, false);
        CHECK(list->CountItems() == 0);
        CHECK(!paste->IsEnabled());
    }
    popup->Quit();
}
int main()
{
    BApplication app("application/x-vnd.airOS-Clipper-UITests");
    CheckPopupPlacement();
    History history;
    for (const char* sample : {"UI review: first clip", "UI review: second clip"}) {
        BMessage data; data.AddData("text/plain", B_MIME_TYPE, sample, strlen(sample));
        history.Capture(data, "UI test");
    }
    auto* window = new TestWindow;
    window->Run();
    window->Lock();
    window->Update(history, false);
    window->Open(); window->Unlock(); snooze(100000); window->Lock();
    window->Dismiss(); window->Dismiss(); window->Open();
    CHECK(!window->IsHidden());
    auto* search = dynamic_cast<BTextControl*>(window->FindView("search"));
    auto* list = dynamic_cast<BListView*>(window->FindView("clips"));
    auto* settings = dynamic_cast<BButton*>(window->FindView("settings"));
    auto* pause = dynamic_cast<BButton*>(window->FindView("pause"));
    CHECK(search && list && settings && pause);
    if (search && list && settings && pause) {
        BMessage down(B_KEY_DOWN); char bytes[] = {B_DOWN_ARROW, 0}; down.AddString("bytes", bytes);
        window->DispatchMessage(&down, search->TextView());
        CHECK(list->CurrentSelection() == 1);
        BMessage enter(B_KEY_DOWN); enter.AddString("bytes", "\n");
        settings->MakeFocus();
        window->DispatchMessage(&enter, settings);
        window->Unlock(); snooze(100000); window->Lock();
        CHECK(window->settingsInvoked);
        window->Update(history, true);
        CHECK(strcmp(pause->Label(), "Resume recording") == 0);
        // Single-pixel dimensions must produce finite image preview geometry.
        BBitmap bitmap(BRect(0, 0, 0, 7), B_RGBA32);
        memset(bitmap.Bits(), 255, bitmap.BitsLength());
        BMessage archive, data; bitmap.Archive(&archive); data.AddMessage("image/bitmap", &archive);
        history.Capture(data, "UI test");
        window->Update(history, false);
        window->Open(); window->UpdateIfNeeded();
        CHECK(strcmp(pause->Label(), "Pause recording") == 0);
    }
    window->Quit();
    CheckPopup(history);
    auto* preferences = new SettingsWindow(100, true, true);
    preferences->Show(); snooze(100000); preferences->Lock();
    CHECK(preferences->FindView("cancel") != nullptr);
    auto* startup = dynamic_cast<BCheckBox*>(preferences->FindView("startup"));
    CHECK(startup && (startup->Value()==B_CONTROL_ON)==StartupEnabled());
    CHECK(preferences->CurrentFocus() == dynamic_cast<BTextControl*>(preferences->FindView("limit"))->TextView());
    preferences->Quit();
    std::printf("UI tests: %d failures\n", failures);
    return failures ? 1 : 0;
}
