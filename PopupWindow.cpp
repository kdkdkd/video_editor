#include "PopupWindow.h"
#include "juce/juce.h"

PopupWindow::PopupWindow(const String& name,
                         const Colour& backgroundColour,
                         int requiredButtons,
                         bool addToDesktop):
    DocumentWindow (name,backgroundColour,requiredButtons,addToDesktop)
{

}


void PopupWindow::closeButtonPressed()
{
    this->removeFromDesktop();
    delete this->getContentComponent();
}
