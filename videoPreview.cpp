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
void _UpdatePreview(void * object)
{

    videoPreviewComponent * o = (videoPreviewComponent *)object;

    if(o->encodedMovie)
    {
        File f(o->encodedMovie->filename);
        if(f.exists())
            f.deleteFile();
        delete o->encodedMovie;
        o->encodedMovie = 0;
    }
    Movie::Info info_copy(o->parent->GetMovieInfo());
    info_copy.filename = File::createTempFile("tmp").getFullPathName();
    String res = o->parent->timeline->Render(info_copy,true);
    if(res!=String::empty)
    {
        o->encodedMovie = 0;
        File f(info_copy.filename);
        if(f.exists())
            f.deleteFile();
        o->repaint();
        return;
    }
    Movie* movie = new Movie();
    movie->Load(info_copy.filename);
    if(!movie->loaded)
    {
        File f(info_copy.filename);
        if(f.exists())
            f.deleteFile();
        delete movie;
        o->encodedMovie = 0;
        o->repaint();
        return;
    }
    o->encodedMovie = movie;
    o->repaint();
}

void  videoPreviewComponent::timerCallback()
{
    if(encodedMovie)
    {
        encodedMovie->ReadAndDecodeFrame();
        repaint();
    }

}
videoPreviewComponent::videoPreviewComponent(encodeVideoComponent* parent):Component()
{
    this->parent = parent;
    AddEvent(parent->mainWindow->AfterChangePosition,this,_UpdatePreview);
    encodedMovie = 0;
}
videoPreviewComponent::~videoPreviewComponent()
{
    if(encodedMovie)
    {
        File f(encodedMovie->filename);
        if(f.exists())
            f.deleteFile();
        delete encodedMovie;
    }

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

        if(encodedMovie)
        {
            image_width = encodedMovie->width;
            image_height = encodedMovie->height;

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
            g.drawImage(*encodedMovie->image,dstX + width/2,dstY,width/2,height,srcX,srcY,width/2,height);
        }


    }
}
