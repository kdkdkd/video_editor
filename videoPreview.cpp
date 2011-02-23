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

void videoPreviewComponent::run()
{

    Movie::Info info_copy;

    double timeline_second;
    {
        const MessageManagerLock mml (Thread::getCurrentThread());
        if (! mml.lockWasGained())
            return;
        if(!parent->timeline->current_interval)
            return;
        info_copy = parent->GetMovieInfo();
        timeline_second = parent->timeline->current - parent->timeline->current_interval->absolute_start;
        timeline_copy->Load(parent->timeline->current_interval->movie->filename,true);
    }
    info_copy.filename = File::createTempFile("tmp").getFullPathName();
    timeline_copy->GotoSecondAndRead(timeline_second,true);
    String res = timeline_copy->Render(info_copy,true);
    if(res!=String::empty)
    {
        const MessageManagerLock mml (Thread::getCurrentThread());
        if (! mml.lockWasGained())
            return;

        encodedMovie = 0;
        File f(info_copy.filename);
        if(f.exists())
            f.deleteFile();
        repaint();
        return;
    }

    Movie* movie = new Movie();
    movie->Load(info_copy.filename,false);
    if(!movie->loaded)
    {
        const MessageManagerLock mml (Thread::getCurrentThread());
        if (! mml.lockWasGained())
            return;

        File f(info_copy.filename);
        if(f.exists())
            f.deleteFile();
        delete movie;
        encodedMovie = 0;
        repaint();
        return;
    }
    const MessageManagerLock mml (Thread::getCurrentThread());
    if (! mml.lockWasGained())
        return;

    encodedSecond = timeline_second;
    timeline_copy->GotoSecondAndRead(timeline_second,true);
    encodedMovie = movie;
    repaint();

}
void _UpdatePreview(void * object)
{
    videoPreviewComponent * o = (videoPreviewComponent *)object;
    if(!o->isThreadRunning())
    {
        o->dirty = false;
        if(o->encodedMovie)
        {
            File f(o->encodedMovie->filename);
            if(f.exists())
                f.deleteFile();
            delete o->encodedMovie;
            o->encodedMovie = 0;
        }
        if(o->timeline_copy)
        {
            delete o->timeline_copy;
        }
        o->timeline_copy = new Timeline();
        o->startThread();
    }
    else
    {
        o->dirty = true;
    }
}

void  videoPreviewComponent::timerCallback()
{
    if(dirty)
    {
        _UpdatePreview(this);
        return;
    }
    if(encodedMovie && !isThreadRunning())
    {
        if(!(encodedMovie->ReadAndDecodeFrame()))
        {
            encodedMovie->GotoSecondAndRead(0.0);
            timeline_copy->GotoSecondAndRead(encodedSecond,true);
        }
        else
        {
            timeline_copy->ReadAndDecodeFrame();
        }
        repaint();
    }

}
videoPreviewComponent::videoPreviewComponent(encodeVideoComponent* parent):Component(), Thread ("PreviewThread")
{
    this->parent = parent;
    AddEvent(parent->mainWindow->AfterChangePosition,this,_UpdatePreview);

    timeline_copy = 0;
    encodedMovie = 0;
    startTimer(200);
    encodedSecond = 0.0;
    dirty = false;

}
videoPreviewComponent::~videoPreviewComponent()
{
    stopTimer();
    stopThread(1000);
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
    videoPreviewComponent *o =((videoPreviewComponent *)getContentComponent());
    o->startTimer(200);
}

void videoPreview::remove()
{
    removeFromDesktop();
    parent->showPreview->setToggleState(false,false);
    parent->isPreviewVisible = false;
    videoPreviewComponent *o =((videoPreviewComponent *)getContentComponent());
    o->stopTimer();
}

void videoPreviewComponent::paint(Graphics& g)
{
    int width = getWidth();
    int height = getHeight();
    if(!parent->timeline->current_interval)
        g.drawImageWithin(*(parent->timeline->GetImage()),0,0,width/2,height,RectanglePlacement::stretchToFit,false);
    else
    {

        if(encodedMovie)
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
            g.drawImage(*(timeline_copy->GetImage()),dstX,dstY,width/2,height,srcX,srcY,width/2,height);

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
            g.drawImage(*(encodedMovie->image),dstX + width/2,dstY,width/2,height,srcX,srcY,width/2,height);
        }


    }
}
