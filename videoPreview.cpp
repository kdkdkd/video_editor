#include "videoPreview.h"
using namespace localization;


videoPreview::videoPreview(encodeVideoComponent* parent):DocumentWindow(LABEL_PREVIEW_VIDEO,Colours::whitesmoke,DocumentWindow::closeButton)
{
    setTitleBarHeight (20);
    setResizable (true, false);
    setBounds(10,10,400,300);
    this->parent = parent;
    setVisible(true);
    add();
}
videoPreview::~videoPreview()
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

void videoPreview::paint(Graphics& g)
{
    g.drawImageWithin(*(parent->timeline->GetImage()),0,0,getWidth()/2,getHeight(),RectanglePlacement::centred,false);

}
