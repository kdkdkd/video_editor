#include "videoPreview.h"
using namespace localization;


videoPreview::videoPreview(encodeVideoComponent* parent):DocumentWindow(LABEL_PREVIEW_VIDEO,Colours::whitesmoke,DocumentWindow::closeButton)
{
    setTitleBarHeight (20);
    setResizable (true, false);
    videoPreviewComponent* contentComponent = new videoPreviewComponent(parent);

    setContentComponent(contentComponent);
    setResizeLimits (400, 200, 8192, 8192);
    setBounds(10,10,600,200);
    this->parent = parent;
    setVisible(true);
    add();
}
videoPreview::~videoPreview()
{

}
void videoPreview::UpdatePreview()
{
    videoPreviewComponent* contentComponent = (videoPreviewComponent*)getContentComponent();
    contentComponent->UpdatePreview();
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
        info_copy.audios.clear();
        timeline_second = parent->timeline->current - parent->timeline->current_interval->absolute_start + parent->timeline->current_interval->start;
        timeline_copy->Load(parent->timeline->current_interval->movie->filename,true);
        timeline_copy->current_interval->start = timeline_second;
        double end = timeline_second + 2.0;
        if(parent->timeline->current_interval->end<end)
        {
            end = parent->timeline->current_interval->end;

        }
        timeline_copy->current_interval->end = end;
        timeline_copy->current_interval->absolute_start = 0.0;
        timeline_copy->RecalculateDuration();
        timeline_copy->RecalculateCurrent();
    }
    info_copy.filename = File::createTempFile("tmp").getFullPathName();
    //Some very very wierd bug
    timeline_copy->GotoSecondAndRead(0.0,true);
    timeline_copy->SkipFrame();
    timeline_copy->GotoSecondAndRead(0.0,true);
    String res = timeline_copy->Render(info_copy,this,0,0);
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
        delete movie;
        encodedMovie = 0;

        File f(info_copy.filename);
        if(f.exists())
            f.deleteFile();
        repaint();
        return;
    }

    const MessageManagerLock mml (Thread::getCurrentThread());
    if (! mml.lockWasGained())
        return;

    timeline_copy->GotoSecondAndRead(0.0,true);
    encodedMovie = movie;
    File f(info_copy.filename);

    estimated_file_size = File::descriptionOfSizeInBytes((double)f.getSize() * parent->timeline->duration / timeline_copy->duration );
    repaint();

}


void _UpdatePreview(void * object)
{
    videoPreviewComponent * o = (videoPreviewComponent *)object;
    if(!o->parent->isPreviewVisible)
        return;
    if(!o->isThreadRunning())
    {
        o->dirty = false;
        if(o->encodedMovie)
        {
            File f(o->encodedMovie->filename);
            delete o->encodedMovie;
            o->encodedMovie = 0;
            if(f.exists())
                f.deleteFile();
        }
        if(o->timeline_copy)
        {
            delete o->timeline_copy;
        }
        o->timeline_copy = new Timeline();
        o->repaint();
        o->startThread();
    }
    else
    {
        o->dirty = true;
    }
}

void videoPreviewComponent::UpdatePreview()
{
    _UpdatePreview(this);
}

void  videoPreviewComponent::timerCallback()
{

    if(encodedMovie && !isThreadRunning() && !dirty)
    {
        if(!(encodedMovie->ReadAndDecodeFrame()))
        {
            encodedMovie->GotoSecondAndRead(0.0);
            timeline_copy->GotoSecondAndRead(0.0,true);
        }
        else
        {
            timeline_copy->ReadAndDecodeFrame();
        }
        repaint();
    }
    else
    {

        label_loading = LABEL_LOADING;
        for(int i = 0; i<label_loading_int; ++i)
        {
            label_loading += " ";
        }
        label_loading += ".";
        label_loading_int = (label_loading_int + 1)%5;

        repaint();
    }
    if(dirty && !isThreadRunning())
    {
        _UpdatePreview(this);
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
    dirty = false;

    label_loading_int = 0;
    UpdatePreview();
}

videoPreviewComponent::~videoPreviewComponent()
{
    stopTimer();
    //stopThread(200000);
    if(encodedMovie)
    {
        File f(encodedMovie->filename);
        delete encodedMovie;
        if(f.exists())
            f.deleteFile();
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
    UpdatePreview();
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
    if(isThreadRunning() || dirty)
    {
        int string_width = g.getCurrentFont().getStringWidth(LABEL_LOADING);
        int string_height = g.getCurrentFont().getHeight();
        g.drawText(label_loading,(width-string_width)/2,(height-string_height)/2,string_width+30,string_height,Justification::centredLeft,false);
        return;
    }


    if(encodedMovie)
    {
        int image_width = parent->timeline->GetImage()->getWidth();
        int image_height = parent->timeline->GetImage()->getHeight();
        int aviable_width = width/2 - 3;
        int font_height = 1.5 * (double)g.getCurrentFont().getHeight();

        int aviable_height = height - 2 * font_height - 4;

        g.drawText(LABEL_VIDEO_PREVIEW_ORIGINAL,15,0,aviable_width,font_height,Justification::centredLeft,false);
        g.drawText(LABEL_VIDEO_PREVIEW_ENCODED,width/2+15,0,aviable_width,font_height,Justification::centredLeft,false);

        g.drawText(LABEL_VIDEO_PREVIEW_ESTIMATED_FILE_SIZE + T(" : ") + estimated_file_size,15,aviable_height + font_height + 2 ,width-20,font_height,Justification::centredLeft,false);

        int dstX = 2;
        int dstY = font_height + 2;
        if(image_width<aviable_width)
        {
            dstX += (aviable_width - image_width)/2;
        }
        if(image_height<aviable_height)
        {
            dstY += (aviable_height - image_height)/2;
        }
        int srcX = 0;
        int srcY = 0;
        if(image_width>aviable_width)
        {
            srcX = (- aviable_width + image_width)/2;
        }
        if(image_height>aviable_height)
        {
            srcY = (- aviable_height + image_height)/2;
        }
        g.drawImage(*(timeline_copy->GetImage()),dstX,dstY,aviable_width,aviable_height,srcX,srcY,aviable_width,aviable_height);

        image_width = encodedMovie->width;
        image_height = encodedMovie->height;

        dstX = 2;
        dstY = font_height + 2;
        if(image_width<aviable_width)
        {
            dstX += (aviable_width - image_width)/2;
        }
        if(image_height<aviable_height)
        {
            dstY += (aviable_height - image_height)/2;
        }
        srcX = 0;
        srcY = 0;
        if(image_width>aviable_width)
        {
            srcX = (- aviable_width + image_width)/2;
        }
        if(image_height>aviable_height)
        {
            srcY = (- aviable_height + image_height)/2;
        }
        g.drawImage(*(encodedMovie->image),dstX + width/2,dstY,aviable_width,aviable_height,srcX,srcY,aviable_width,aviable_height);
    }else
    {
        g.drawText(LABEL_VIDEO_PREVIEW_FAILED,0,0,width,height/2,Justification::centredBottom,true);
        g.drawText(LABEL_VIDEO_PREVIEW_FAILED_TOOLTIP,0,height/2,width,height/2,Justification::centredTop,true);
    }



}
