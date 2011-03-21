#ifndef FIRST_PAGE_H_INCLUDED
#define FIRST_PAGE_H_INCLUDED
#include "config.h"
#include "MainComponent.h"
#include "juce/juce.h"

class MainComponent;
class SpecialButton;
class firstPage:public Component,public ButtonListener
{
    public:
        void paint(Graphics &g);
        firstPage(MainComponent* main);
        ~firstPage();
        MainComponent* main;
        void buttonClicked(Button* button);
        Array<SpecialButton*> recent_list;
        void resized();

};

#endif
