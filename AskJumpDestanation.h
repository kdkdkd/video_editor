#ifndef _ASKJUMPDESTANATION_H_
#define _ASKJUMPDESTANATION_H_

#include <juce.h>
#include "MainComponent.h"

class AskJumpDestanation : public AlertWindow
{
private:
    MainComponent* mainWindow;
public:
    int result;

    TextEditor * input_hours;
    TextEditor * input_minutes;
    TextEditor * input_seconds;
    TextEditor * input_microseconds;

    TextButton * okButton;
    TextButton * cancelButton;

    void paint (Graphics& g);

    void buttonClicked (Button* button);

    AskJumpDestanation(MainComponent* mainWindow_);

    ~AskJumpDestanation();

};

#endif//_ASKJUMPDESTANATION_H_
