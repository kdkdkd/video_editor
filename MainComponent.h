#ifndef _MAINCOMPONENT_H_
#define _MAINCOMPONENT_H_


#include "MainAppWindow.h"
#include <juce.h>
#include "movie.h"
#include "localization.h"
#include "toolbox.h"

class AskJumpDestanation;

using namespace localization;

class MainComponent : public Component, public ButtonListener, public MenuBarModel, public ApplicationCommandTarget, public Thread, public Timer
{
public:
    String filename;
    bool file_choosed;

    int current_frame;
    int mouse_x;
    int mouse_y;
    double load_progress;
    Movie *movie_new;
    String new_filename_cur;
    bool loaded_something;
    int64 operation_start;
    int64 operation_estimate;
    String last_estimation;
    AskJumpDestanation *ask_jump_target;

    void changeFileName(String new_filename);

    void run();

    TooltipWindow tooltipWindow;
    TextButton* myButton;
    Label* filename_label;
    Label* estimate_label;
    ProgressBar* progress;

    void timerCallback();

    enum CommandIDs
    {
        commandOpen                 = 0x2000,
        commandSave                 = 0x2001,
        commandEncode               = 0x2002,
        commandJump                 = 0x2003,
        commandSaveFrame            = 0x2004

    };


    MainAppWindow* mainWindow;

    Movie *movie;
    void buttonClicked (Button* button);
    MainComponent (MainAppWindow* mainWindow_);

    ~MainComponent ();

    void resized ();

    void paint (Graphics& g);
    int GetArrowPosition();
    bool NeedDrawArrow();

    int GetFrameFromPosition(int position);
    int GetPositionByFrame(int frame);

    void DrawSlider(Graphics& g);

    void DrawArrow(Graphics& g);

    void repaintSlider();

    void mouseMove (const MouseEvent& e);
    void mouseDown (const MouseEvent& e);
    const StringArray getMenuBarNames();
    const PopupMenu getMenuForIndex (int menuIndex, const String& menuName);

    void menuItemSelected (int menuItemID, int topLevelMenuIndex);

    bool perform (const InvocationInfo& info);
    ApplicationCommandTarget* getNextCommandTarget();

    void getAllCommands (Array <CommandID>& commands);
    bool isVideoReady ();
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result);
};

#endif//_MAINCOMPONENT_H_
