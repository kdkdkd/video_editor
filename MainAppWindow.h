#ifndef _MainAppWindow_H__
#define _MainAppWindow_H__

#include "MainComponent.h"
#include "juce/juce.h"

class MainComponent;
class MainAppWindow  : public DocumentWindow
{
public:

    MainAppWindow();
    ~MainAppWindow();

    void closeButtonPressed();

    ApplicationCommandManager* commandManager;

    MainComponent * contentComponent;

};


#endif
