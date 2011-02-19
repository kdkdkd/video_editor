#include "videoPreview.h"
using namespace localization;


videoPreview::videoPreview(encodeVideoComponent* parent):DocumentWindow(LABEL_PREVIEW_VIDEO,Colours::whitesmoke,DocumentWindow::closeButton)
{
    setTitleBarHeight (20);
    setResizable (true, false);
    videoPreviewComponent* contentComponent = new videoPreviewComponent(parent);

    setContentComponent(contentComponent);
    setBounds(10,10,600,200);
    this->parent = parent;
    setVisible(true);
    add();
}
videoPreview::~videoPreview()
{

}
void _DoOnMainComponentPointerChange(void * object)
{
    videoPreviewComponent * o = (videoPreviewComponent *)object;
    o->repaint();
}

videoPreviewComponent::videoPreviewComponent(encodeVideoComponent* parent):Component()
{
    this->parent = parent;
    AddEvent(parent->mainWindow->AfterChangePosition,this,_DoOnMainComponentPointerChange);
}
videoPreviewComponent::~videoPreviewComponent()
{

}
void videoPreview::closeButtonPressed()
{
    remove();
}

void videoPreview::add()
{
    addToDesktop(ComponentPeer::windowHasCloseButton || ComponentPeer::windowHasTitleBar || ComponentPeer::windowIsResizable);
}

void videoPreview::remove()
{
    removeFromDesktop();
    parent->showPreview->setToggleState(false,false);
    parent->isPreviewVisible = false;
}

void videoPreviewComponent::paint(Graphics& g)
{
    g.drawImageWithin(*(parent->timeline->GetImage()),0,0,getWidth()/2,getHeight(),RectanglePlacement::centred,false);

}
