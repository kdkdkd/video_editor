#ifndef _PopupWindow_H__
#define _PopupWindow_H__

#include <juce.h>


class PopupWindow  : public DocumentWindow
{
public:
    PopupWindow(const String& name,const Colour& backgroundColour,int requiredButtons,bool addToDesktop = true);

    void closeButtonPressed();
};


#endif
