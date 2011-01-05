#ifndef CONTAINER_BOX_H
#define	CONTAINER_BOX_H

#include "juce/juce.h"

class ContainerBox  : public Viewport

{
private:
    int currentHeight;

public:
    void addChildComponent (Component* child, int zOrder = -1);
    void addAndMakeVisible (Component* child, int zOrder = -1);
    ContainerBox(const String& name);
    int getIndexOfChildComponent(const Component* const child);
    void resized ();
    ~ContainerBox();
};






#endif
