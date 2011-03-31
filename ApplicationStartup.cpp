#include "config.h"
#include "MainAppWindow.h"


class AppClass : public JUCEApplication
{

    MainAppWindow* theMainWindow;

public:

    AppClass() : theMainWindow (0)
    {

    }

    ~AppClass()
    {

    }


    void initialise (const String& commandLine)
    {

        theMainWindow = new MainAppWindow();


        theMainWindow->centreWithSize (800, 600);

        theMainWindow->setVisible (true);


    }

    void shutdown()
    {

        deleteAndZero (theMainWindow);
    }


    const String getApplicationName()
    {

        return T("Video Editor");
    }

    const String getApplicationVersion()
    {

        return T("0.3.2");
    }

    bool moreThanOneInstanceAllowed()
    {

        return true;
    }

    void anotherInstanceStarted (const String& commandLine)
    {

    }
};


START_JUCE_APPLICATION(AppClass)
