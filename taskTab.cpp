#include "taskTab.h"


taskTab::taskTab():DocumentWindow(LABEL_TASK_TAB,Colours::whitesmoke,DocumentWindow::closeButton)
{
    setTitleBarHeight (20);
    setResizable (false, false);
    setBounds(30,30,600,600);
}
void taskTab::add()
{
    setVisible(true);
    addToDesktop(ComponentPeer::windowHasCloseButton || ComponentPeer::windowHasTitleBar);
}
void taskTab::remove()
{
    removeFromDesktop();
}

taskTab::~taskTab()
{

}

void taskTab::closeButtonPressed()
{
    remove();
}
