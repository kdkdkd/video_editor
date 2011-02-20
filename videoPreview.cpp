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
    o->encodedImage = o->parent->timeline->RenderImage(o->parent->GetMovieInfo());
    o->repaint();
}

videoPreviewComponent::videoPreviewComponent(encodeVideoComponent* parent):Component()
{
    this->parent = parent;
    AddEvent(parent->mainWindow->AfterChangePosition,this,_DoOnMainComponentPointerChange);
    encodedImage = Image();
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
    int width = getWidth();
    int height = getHeight();
    if(!parent->timeline->current_interval)
        g.drawImageWithin(*(parent->timeline->GetImage()),0,0,width/2,height,RectanglePlacement::stretchToFit,false);
    else
    {
        int image_width = parent->timeline->GetImage()->getWidth();
        int image_height = parent->timeline->GetImage()->getHeight();
        int dstX = 0;
        int dstY = 0;
        if(image_width<width/2)
        {
            dstX = (width/2 - image_width)/2;
        }
        if(image_height<height)
        {
            dstY = (height - image_height)/2;
        }
        int srcX = 0;
        int srcY = 0;
        if(image_width>width/2)
        {
            srcX = (- width/2 + image_width)/2;
        }
        if(image_height>height)
        {
            srcY = (- height + image_height)/2;
        }
        g.drawImage(*(parent->timeline->GetImage()),dstX,dstY,width/2,height,srcX,srcY,width/2,height);

        image_width = encodedImage.getWidth();
        image_height = encodedImage.getHeight();
        if(image_width>0)
        {
            dstX = 0;
            dstY = 0;
            if(image_width<width/2)
            {
                dstX = (width/2 - image_width)/2;
            }
            if(image_height<height)
            {
                dstY = (height - image_height)/2;
            }
            srcX = 0;
            srcY = 0;
            if(image_width>width/2)
            {
                srcX = (- width/2 + image_width)/2;
            }
            if(image_height>height)
            {
                srcY = (- height + image_height)/2;
            }
            g.drawImage(encodedImage,dstX + width/2,dstY,width/2,height,srcX,srcY,width/2,height);
        }


    }
}
