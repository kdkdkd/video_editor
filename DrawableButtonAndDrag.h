#ifndef _DrawableButtonAndDrag_H__
#define _DrawableButtonAndDrag_H__

#include "juce/juce.h"
#include "timeline.h"
#include "ContainerBox.h"
#include "MainComponent.h"

class DrawableButtonAndDrag : public DrawableButton
{
    public:
    DrawableButtonAndDrag (const String& buttonName,ButtonStyle buttonStyle,Timeline* timeline,ContainerBox * box,MainComponent * main);
    void mouseDrag (const MouseEvent& e);
    Timeline* timeline;
    ContainerBox* box;
    MainComponent * main;
};

#endif
