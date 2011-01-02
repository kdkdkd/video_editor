#ifndef _MAINCOMPONENT_H_
#define _MAINCOMPONENT_H_


#include "MainAppWindow.h"
#include <juce.h>
#include "timeline.h"
#include "localization.h"
#include "toolbox.h"

class AskJumpDestanation;

using namespace localization;

class MainComponent : public Component, public MenuBarModel, public ApplicationCommandTarget, public Timer, public ButtonListener
{
public:
    int mouse_x;
    int mouse_y;
    AskJumpDestanation *ask_jump_target;

    void changeFileName(String new_filename);
    TooltipWindow tooltipWindow;
    DrawableButton* playButton;
    DrawableButton* pauseButton;
    DrawableButton* stopButton;
    DrawableButton* nextFrameButton;
    DrawableButton* prevFrameButton;

    void initImageButton(String pic_name,DrawableButton*& button);

    void SetVisibleButtons(bool visible);


    enum CommandIDs
    {
        commandOpen                 = 0x2000,
        commandSave                 = 0x2001,
        commandEncode               = 0x2002,
        commandJump                 = 0x2003,
        commandSaveFrame            = 0x2004,
        commandPlay                 = 0x2005,
        commandPause                = 0x2006,
        commandStop                 = 0x2007,
        commandNextFrame            = 0x2008,
        commandPrevFrame            = 0x2009,
        commandNext5Frame           = 0x200A,
        commandPrev5Frame           = 0x200B,
        commandNextSecond           = 0x200C,
        commandPrevSecond           = 0x200D,
        commandInfo                 = 0x200E

    };


    MainAppWindow* mainWindow;

    Timeline *timeline;
    void buttonClicked (Button* button);
    MainComponent (MainAppWindow* mainWindow_);

    ~MainComponent ();

    void resized ();

    void paint (Graphics& g);
    int GetArrowPosition();
    int GetCurrentPosition();
    bool NeedDrawArrow();
    void timerCallback();
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

    int64 miliseconds_start;
    bool video_playing;
    void StopVideo();
    void StartVideo();
};

#endif//_MAINCOMPONENT_H_
