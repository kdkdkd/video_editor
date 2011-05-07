#include "config.h"
#include "MainComponent.h"
#include "PopupWindow.h"
#include "AskJumpDestanation.h"
#include "DrawableButtonAndDrag.h"
#include "encodeVideo.h"
#include "capabilities.h"
#include "tasks.h"
#include <math.h>
#define VIDEO_TIMELINE_SIZE 98
#define AUDIO_TIMELINE_SIZE 30

#define TIMELINE_OFFSET 84


void MainComponent::changeFileName(String new_filename)
{
    bool loaded_local = false;
    Movie * movie = 0;
    Sound * sound = 0;
    timeline->Load(new_filename,false,movie,sound);
    loaded_local = timeline->loaded;
    if(loaded_local)
    {
        recent.addFile(new_filename);
        properties.setValue("recent",recent.toString());
        if(first)
        {
            delete first;
            first = 0;
        }
        SetVisibleButtons(true);
        if(movie)
            AddMovieToList(movie);
        sliderValueChanged(scale_timeline);
        ResizeViewport();
        repaint();
    }
    else
    {
        AlertWindow::showMessageBox (AlertWindow::WarningIcon,CANT_LOAD_FILE,new_filename);
    }
}

void MainComponent::startEmpty()
{
    timeline->loaded = true;
    if(first)
    {
        delete first;
        first = 0;
    }
    SetVisibleButtons(true);
    sliderValueChanged(scale_timeline);
    ResizeViewport();
    repaint();
}

void MainComponent::AddMovieToList(Movie*movie)
{
    Component *preview = new Component();
    preview->setSize(124,96+42);


    DrawableButtonAndDrag *button_preview = new DrawableButtonAndDrag("",DrawableButton::ImageFitted,timeline,movies_list,this);

    button_preview->addListener(this);
    DrawableImage normal,over;

    normal.setImage(*movie->image_preview);
    normal.setOpacity(0.9);
    over.setImage(*movie->image_preview);

    button_preview->setImages (&normal, &over, &over);
    button_preview->addListener(this);
    button_preview->setBounds(0,0,124,96);
    preview->addAndMakeVisible(button_preview);
    File f(movie->filename);
    String file_name = f.getFileName();
    Label *caption = new Label(file_name,file_name);
    caption->setTooltip(movie->filename);

    Font font = caption->getFont();
    font.setHeight(14);
    caption->setFont(font);
    caption->setBounds(0,96,124,42);
    caption->setJustificationType(Justification::centredTop);
    preview->addAndMakeVisible(caption);
    movies_list->addAndMakeVisible(preview);

}

void MainComponent::SetVisibleButtons(bool visible)
{
    playButton->setVisible(visible);
    pauseButton->setVisible(visible);
    zoomInButton->setVisible(visible);
    zoomOutButton->setVisible(visible);

    prevFrameButton->setVisible(visible);
    nextFrameButton->setVisible(visible);
    stopButton->setVisible(visible);
    movies_list->setVisible(visible);
    scale_timeline->setVisible(visible);
    tabs_data->setVisible(visible);
}

void MainComponent::ResizeViewport()
{
    int height_current = getHeight();
    int movies_border = 300;
    if(isVideoReady())
    {
        movies_border = GetMoviesBorder();
    }
    tabs_data->setBounds(13,1,movies_border-15,height_current - 232 - TIMELINE_OFFSET);

}

void  MainComponent::timerCallback()
{
    stopTimer();

    int width_prev = timeline->GetImage()->getWidth();
    int height_prev = timeline->GetImage()->getHeight();
    timeline->ReadAndDecodeFrame();
    if(timeline->GetImage()->getWidth()!=width_prev || timeline->GetImage()->getHeight() != height_prev)
        ResizeViewport();
    repaint();

    int spend = Time::getCurrentTime().toMilliseconds()-miliseconds_start;
    if(miliseconds_start<0)
        spend = 0;
    int need = 1000.0 / timeline->GetFps();

    int timer = need - spend;

    if(timer<1)
        timer = 1;

    miliseconds_start = Time::getCurrentTime().toMilliseconds();
    startTimer(timer);

}

void MainComponent::buttonClicked (Button* button)
{


    ApplicationCommandManager* const commandManager = mainWindow->commandManager;
    if(button==zoomOutButton)
        {
            double value = scale_timeline->getValue();
            value+=0.5;
            if(value>10.0)
                value = 10.0;
            scale_timeline->setValue(value);
        }else
    if(button==zoomInButton)
        {
            double value = scale_timeline->getValue();
            value-=0.5;
            if(value<0.0)
                value = 0.0;
            scale_timeline->setValue(value);
        }else
    if(button==playButton)
        commandManager->invokeDirectly(commandPlay,false);
    else if(button == pauseButton)
        commandManager->invokeDirectly(commandPause,false);
    else if(button == prevFrameButton)
        commandManager->invokeDirectly(commandPrevFrame,false);
    else if(button == nextFrameButton)
        commandManager->invokeDirectly(commandNextFrame,false);
    else if(button == stopButton)
        commandManager->invokeDirectly(commandStop,false);
    else
    {
        int index = movies_list->getIndexOfChildComponent(button->getParentComponent());
        Movie*movie = timeline->movies[index];


        {
            PopupMenu context_menu;
            context_menu.addItem(1000, LABEL_INFO, true, false);
            context_menu.addItem(1001, LABEL_DELETE, true, false);
            Rectangle<int> area=button->getScreenBounds();
            area.setHeight(0);
            int result = context_menu.showAt(area);
            switch(result)
            {
            case 1000:
                toolbox::show_info_popup(LABEL_INFO,movie->PrintMovieInfo(),this);
                break;
            case 1001:
                Component *viewed = button->getParentComponent();
                int num = viewed->getNumChildComponents();
                for(int i = 0; i<num; ++i)
                {
                    Component *child = viewed->getChildComponent(0);
                    viewed->removeChildComponent(child);
                    delete child;
                }
                movies_list->getViewedComponent()->removeChildComponent(button->getParentComponent());
                delete viewed;

                movies_list->resized();
                timeline->movies.erase(timeline->movies.begin()+index);
                break;
            }

        }

    }


}
void MainComponent::initImageButton(String pic_name,DrawableButton*& button)
{
    button = new DrawableButton("",DrawableButton::ImageFitted);
    DrawableImage normal,over;
    Image image = ImageCache::getFromFile(pic_name);
    normal.setImage (image);
    normal.setOpacity(0.8);
    over.setImage (image);
    button->setImages (&normal, &over, &normal);
    button->addListener(this);
    addChildComponent(button);
}

void MainComponent::scrollBarMoved (ScrollBar* scrollBarThatHasMoved,double newRangeStart)
{
    timeline_position = newRangeStart;
    repaintSlider();
}

void MainComponent::sliderValueChanged(Slider* slider)
{
    double second_to_pixel_prev = second_to_pixel;
    second_to_pixel = slider->getValue()*(0.14620516440220848680255318407494) + 0.31622776601683793319988935444327;
    second_to_pixel*=second_to_pixel;
    second_to_pixel*=second_to_pixel;

    double width = (double)(getWidth()-65);
    if(timeline_position>0.0)
    timeline_position -= (width/second_to_pixel - width/second_to_pixel_prev)/2.0;
    if(timeline_position<0.0)
        timeline_position = 0.0;
    timeline_scrollbar->setRangeLimits(0.0,1.3*timeline->duration);
    timeline_scrollbar->setCurrentRange(timeline_position,(double)(getWidth()-65)/second_to_pixel);



    repaintSlider();
}

void UpdateGuiAfterLocalizationChangedInMainComponent(void *object)
{
    MainComponent * main = (MainComponent *)object;
    main->tabs_data->setTabName(0,LABEL_MOVIES);
    main->tabs_data->setTabName(1,LABEL_SOUNDS);
    main->tabs_data->setTabName(2,LABEL_PICTURES);
    main->scale_timeline->setTooltip(LABEL_SCALE);
    main->properties.setValue("lastLocale",localization::current_locale);
}

MainComponent::MainComponent (MainAppWindow* mainWindow_):properties(File("..\\config\\config.xml"),0,PropertiesFile::storeAsXML,0)
{
    mainWindow = mainWindow_;
    load_locale(properties.getValue("lastLocale","en"));
    recent.restoreFromString(properties.getValue("recent",""));

    av_register_all();
    //av_log_set_level(AV_LOG_DEBUG);
    capabilities::InitFormats();

    tasks = 0;

    initImageButton(String("..\\pic\\zoomin.png"),zoomInButton);
    initImageButton(String("..\\pic\\zoomout.png"),zoomOutButton);

    initImageButton(String("..\\pic\\play.png"),playButton);
    initImageButton(String("..\\pic\\pause.png"),pauseButton);

    initImageButton(String("..\\pic\\prev.png"),prevFrameButton);
    initImageButton(String("..\\pic\\next.png"),nextFrameButton);
    initImageButton(String("..\\pic\\stop.png"),stopButton);


    sound_image = ImageCache::getFromFile(File("..\\pic\\sound.png"));


    timeline = new Timeline();

    movies_list = new ContainerBox("movies_list");
    tabs_data = new TabbedComponent(TabbedButtonBar::TabsAtTop);
    addChildComponent(tabs_data);
    tabs_data->addTab(LABEL_MOVIES,Colours::whitesmoke,movies_list,false);
    tabs_data->addTab(LABEL_SOUNDS,Colours::whitesmoke,new Component(),false);
    tabs_data->addTab(LABEL_PICTURES,Colours::whitesmoke,new Component(),false);



    timeline_scrollbar = new ScrollBar(false,true);
    timeline_scrollbar->addListener(this);
    addChildComponent(timeline_scrollbar);


    second_to_pixel = 2.0;
    timeline_position = 0.0;
    scale_timeline = new Slider("scale_timeline");
    scale_timeline->setSliderStyle(Slider::LinearHorizontal);
    scale_timeline->setTextBoxStyle(Slider::NoTextBox,true,0,0);
    scale_timeline->addListener(this);
    scale_timeline->setValue(second_to_pixel);
    scale_timeline->setTooltip(LABEL_SCALE);
    addChildComponent(scale_timeline);

    ask_jump_target = 0;

    video_playing = false;
    miliseconds_start = -1;

    current_drag_x = -1;
    timeline_original = 0;
    encodeVideoWindow = 0;


    AddEvent(AfterLocalizationChnaged,this,UpdateGuiAfterLocalizationChangedInMainComponent);


    first = new firstPage(this);
    addAndMakeVisible(first);
    init_sound(timeline);
}

MainComponent::~MainComponent()
{
    StopVideo();
    close_sound();

    AfterChangePosition.clear();
    Component *container = movies_list->getViewedComponent();
    int container_num = container->getNumChildComponents();
    for(int i=0; i<container_num; ++i)
    {
        Component *viewed = container->getChildComponent(0);
        int num = viewed->getNumChildComponents();
        for(int i = 0; i<num; ++i)
        {
            Component *child = viewed->getChildComponent(0);
            viewed->removeChildComponent(child);
            delete child;
        }
        movies_list->getViewedComponent()->removeChildComponent(viewed);
        delete viewed;
    }
    delete movies_list;

    delete timeline;
    if(ask_jump_target)
    {
        delete ask_jump_target;
        ask_jump_target = 0;
    }

    if(encodeVideoWindow)
    {
        delete encodeVideoWindow;
        encodeVideoWindow = 0;
    }
    if(tasks)
        delete tasks;
    deleteAllChildren();
    Thread::stopAllThreads(20000);
}


void MainComponent::resized ()
{
    int width_current = getWidth();
    int height_current = getHeight();
    zoomOutButton->setBounds (width_current - 10 - 120 - 40 + 10 , height_current-195-25-TIMELINE_OFFSET + 15, 30, 30);
    zoomInButton->setBounds (width_current - 10 - 120 - 100 - 40 - 30, height_current-195-25-TIMELINE_OFFSET + 15, 30, 30);

    playButton->setBounds (10, height_current-195-25-TIMELINE_OFFSET, 60, 65);
    pauseButton->setBounds (70, height_current-195-25-TIMELINE_OFFSET, 60, 65);
    stopButton->setBounds (130, height_current-195-25-TIMELINE_OFFSET, 60, 65);
    prevFrameButton->setBounds (width_current - 10 - 60 -60, height_current-195-25-TIMELINE_OFFSET, 60, 65);
    nextFrameButton->setBounds (width_current - 10 - 60, height_current-195-25-TIMELINE_OFFSET, 60, 65);
    scale_timeline->setBounds (width_current - 10 - 120 - 100 - 40, height_current-195-30-TIMELINE_OFFSET+3, 110, 65);

    timeline_scrollbar->setBounds ( 40, height_current-25, width_current - 65, 18);
    ResizeViewport();

    if(first)
        first->setBounds(0,0,width_current,height_current);
}

int MainComponent::GetMoviesBorder()
{
    int width_current = getWidth();
    int height_current = getHeight();

    int width_image = timeline->GetImage()->getWidth();
    int height_image = timeline->GetImage()->getHeight();

    int res = 300;
    float scalex = (float)(width_current-310.0f)/(float)width_image;
    float scaley = (height_current-230.0f - TIMELINE_OFFSET)/(float)height_image;
    if(scaley<scalex)
    {
        res += width_current - 310  - (int)(width_image*scaley)-10;
    }
    return res;
}

void MainComponent::paint (Graphics& g)
{
    if(isVideoReady())
    {
        g.setImageResamplingQuality(Graphics::lowResamplingQuality);

        int width_current = getWidth();
        int height_current = getHeight();

        int width_image = timeline->GetImage()->getWidth();
        int height_image = timeline->GetImage()->getHeight();

        float scalex = (width_current-310.0f )/(float)width_image;
        float scaley = (height_current-230.0f - TIMELINE_OFFSET)/(float)height_image;
        float scale = scalex;
        float deltax = 305.0f;
        float deltay = 0.0f;
        if(scaley<scalex)
        {
            scale = scaley;
            deltax += ((float)width_current - 310.0f  - (float)width_image*scale) - 5.0f;
        }
        else
        {
            deltay += ((float)height_current - 230.0f - TIMELINE_OFFSET - (float)height_image*scale)/2.0f;
        }

        g.drawImageWithin(*(timeline->GetImage()),deltax,deltay,(width_image * scale),(height_image * scale) ,RectanglePlacement::centred,false);

        g.setColour(Colour::fromRGB(70,70,70));

        g.drawRect(40,height_current-75 - 30- TIMELINE_OFFSET,width_current-65,VIDEO_TIMELINE_SIZE,1);
        g.drawRect(40,height_current-75 - 30 + VIDEO_TIMELINE_SIZE - 1 - TIMELINE_OFFSET,width_current-65,AUDIO_TIMELINE_SIZE,1);

        g.drawVerticalLine(10,height_current-125 - 30 - TIMELINE_OFFSET ,height_current-25-30 + 19- TIMELINE_OFFSET+ VIDEO_TIMELINE_SIZE - 50 + AUDIO_TIMELINE_SIZE - 20);

        g.drawHorizontalLine(height_current-125 - 30- TIMELINE_OFFSET,10,width_current-120);

        g.drawHorizontalLine(height_current-37- TIMELINE_OFFSET + VIDEO_TIMELINE_SIZE - 50 + AUDIO_TIMELINE_SIZE - 20,10,40);

        g.drawText(LABEL_TIME + String("   ") + toolbox::format_duration(timeline->current) + String(" / ") + toolbox::format_duration(timeline->duration),width_current-520,height_current-125-30 - TIMELINE_OFFSET,400,20,Justification::centredRight,true);

        double timeline_duration = (double)(width_current-65-1)/second_to_pixel;
        //TimeLine
        int number_of_lines = 0;
        double display_interval = 120.0 / second_to_pixel ;
        if(display_interval<60.0)
        {
            display_interval = ((int)display_interval/10) * 10.0;
            number_of_lines = 5;
        }
        else if(display_interval<600.0)
        {
            display_interval = ((int)display_interval/60) * 60.0;
            number_of_lines = 4;
        }
        else
        {
            display_interval = ((int)display_interval/600) * 600.0;
            number_of_lines = 3;
        }

        int x = (int)ceilf(timeline_position / display_interval);

        double label_time = (display_interval * (double)x);
        int label_position = (label_time - timeline_position)*second_to_pixel;
        int safe_space = g.getCurrentFont().getStringWidth("99:99:99")/second_to_pixel;

        g.setColour(Colour::fromRGB(70,70,70));
        for(;;)
        {

            if(label_time<timeline_position+timeline_duration)
            {
                g.drawVerticalLine(40 + label_position,height_current-36 - TIMELINE_OFFSET+ VIDEO_TIMELINE_SIZE - 50 + AUDIO_TIMELINE_SIZE - 20,height_current-36 + 26 - TIMELINE_OFFSET+ VIDEO_TIMELINE_SIZE - 50 + AUDIO_TIMELINE_SIZE - 20);
                String label = toolbox::format_duration_small(label_time);
                if(g.getCurrentFont().getStringWidth(label) + 43+label_position < width_current)
                    g.drawText(label,43+label_position,height_current-36 + 16 - TIMELINE_OFFSET+ VIDEO_TIMELINE_SIZE - 50 + AUDIO_TIMELINE_SIZE - 20,100,10,Justification::centredLeft,true);
            }

            if(label_time <timeline_position+timeline_duration + display_interval)
            {
                double line_interval = -display_interval * second_to_pixel/(double)(number_of_lines + 1);
                double current_position_line = label_position + line_interval;
                for(int i=0; i<number_of_lines; ++i)
                {
                    if(current_position_line<0)
                        break;
                    if(current_position_line<width_current - 65 - 1)
                        g.drawVerticalLine(40 + current_position_line,height_current-36 - TIMELINE_OFFSET+ VIDEO_TIMELINE_SIZE - 50 + AUDIO_TIMELINE_SIZE - 20,height_current-36 + 10 - TIMELINE_OFFSET+ VIDEO_TIMELINE_SIZE - 50 + AUDIO_TIMELINE_SIZE - 20);
                    current_position_line += line_interval;
                }
            }
            else
                break;
            label_time += display_interval;
            label_position = (label_time - timeline_position)*second_to_pixel;


        }

        //~TimeLine


        //List of video intervals

        g.saveState();
        for(vector<Timeline::Interval*>::iterator it = timeline->intervals_video.begin(); it!=timeline->intervals_video.end(); it++)
        {
            double start=timeline_position,end = timeline_position + timeline_duration,start1=(*it)->absolute_start,end1 = (*it)->GetAbsoluteEnd();
            if(start1<=end&&end1>=start)
            {
                int start_position_interval = (-timeline_position + (*it)->absolute_start) * second_to_pixel;
                if(start_position_interval<0)
                    start_position_interval = 0;

                int end_position_interval = (-timeline_position + (*it)->GetAbsoluteEnd()) * second_to_pixel;
                if(end_position_interval>width_current-65-1)
                    end_position_interval = width_current-65-1;

                g.setColour(Colour::fromRGB(70,70,70));

                g.drawRect(start_position_interval + 40,height_current - 75 - 30 - TIMELINE_OFFSET,end_position_interval - start_position_interval + 1,VIDEO_TIMELINE_SIZE,1);

                switch((*it)->color)
                {
                case Timeline::Interval::usual:
                    g.setColour(Colour::fromRGB(200,200,250));
                    break;
                    /*case Timeline::Interval::dragg:
                        g.setColour(Colour::fromRGB(100,100,150));
                        break;*/
                case Timeline::Interval::over:
                    g.setColour(Colour::fromRGB(200,200,100));
                    break;
                case Timeline::Interval::select:
                case Timeline::Interval::dragg:
                    g.setColour(Colour::fromRGB(180,70,70));
                    break;
                }

                g.fillRect(start_position_interval+40+1,height_current-74-30- TIMELINE_OFFSET,end_position_interval - start_position_interval - 1,VIDEO_TIMELINE_SIZE/2-1);

                switch((*it)->color)
                {
                case Timeline::Interval::usual:
                    g.setColour(Colour::fromRGB(180,180,230));
                    break;
                    /*case Timeline::Interval::dragg:
                        g.setColour(Colour::fromRGB(80,80,130));
                        break;*/
                case Timeline::Interval::over:
                    g.setColour(Colour::fromRGB(180,180,80));
                    break;
                case Timeline::Interval::select:
                case Timeline::Interval::dragg:
                    g.setColour(Colour::fromRGB(160,50,50));
                    break;
                }




                g.fillRect(start_position_interval+40+1,height_current-50-30- TIMELINE_OFFSET + (VIDEO_TIMELINE_SIZE-50)/2,end_position_interval - start_position_interval - 1,VIDEO_TIMELINE_SIZE/2-1);

                String label = (*it)->movie->filename;
                File f(label);
                label = f.getFileName();
                g.setColour(Colour::fromRGB(50,50,50));
                int took_space = 4*(VIDEO_TIMELINE_SIZE-2)/3;
                if(took_space>end_position_interval - start_position_interval)
                {
                    took_space = 0;
                }
                if(took_space)
                    g.drawImageWithin(*((*it)->preview),start_position_interval+41,height_current - 75 - 30 - TIMELINE_OFFSET+1,4*(VIDEO_TIMELINE_SIZE-2)/3,VIDEO_TIMELINE_SIZE-2,RectanglePlacement::centred,false);
                //g.drawImageWithin(*((*it)->movie->image_preview),0,0,64,50 ,RectanglePlacement::centred,false);
                g.drawFittedText(label + String(" [") + toolbox::format_duration((*it)->start) + String("  ; ") + toolbox::format_duration((*it)->end) + String("]"),start_position_interval + 50 + took_space,height_current - 75 - 30 - TIMELINE_OFFSET,end_position_interval - start_position_interval - 20 - took_space,VIDEO_TIMELINE_SIZE,Justification::centredLeft,6);

            }
        }
        g.restoreState();
        //~List of video intervals

        //List of sound intervals
        for(vector<Timeline::Interval*>::iterator it = timeline->intervals_audio.begin(); it!=timeline->intervals_audio.end(); it++)
        {
            double start = timeline_position,end = timeline_position + timeline_duration,start1=(*it)->absolute_start,end1 = (*it)->GetAbsoluteEnd();
            if(start1<=end&&end1>=start)
            {
                int start_position_interval = (-timeline_position + start1) * second_to_pixel;
                if(start_position_interval<0)
                    start_position_interval = 0;

                int end_position_interval = (-timeline_position + end1) * second_to_pixel;
                if(end_position_interval>width_current-65-1)
                    end_position_interval = width_current-65-1;

                g.setColour(Colour::fromRGB(70,70,70));

                g.drawRect(start_position_interval + 40,height_current - 75 - 30 + VIDEO_TIMELINE_SIZE - 1 - TIMELINE_OFFSET,end_position_interval - start_position_interval + 1,AUDIO_TIMELINE_SIZE,1);

                switch((*it)->color)
                {
                case Timeline::Interval::usual:
                    g.setColour(Colour::fromRGB(220,220,220));
                    break;

                case Timeline::Interval::over:
                    g.setColour(Colour::fromRGB(230,230,160));
                    break;
                case Timeline::Interval::select:
                case Timeline::Interval::dragg:
                    g.setColour(Colour::fromRGB(210,130,130));
                    break;
                }

                g.fillRect(start_position_interval+40+1,height_current-50-30+25- TIMELINE_OFFSET + VIDEO_TIMELINE_SIZE-50,end_position_interval - start_position_interval - 1,AUDIO_TIMELINE_SIZE/2-1);

                switch((*it)->color)
                {
                case Timeline::Interval::usual:
                    g.setColour(Colour::fromRGB(210,210,210));
                    break;

                case Timeline::Interval::over:
                    g.setColour(Colour::fromRGB(210,210,140));
                    break;
                case Timeline::Interval::select:
                case Timeline::Interval::dragg:
                    g.setColour(Colour::fromRGB(190,110,110));
                    break;
                }




                g.fillRect(start_position_interval+40+1,height_current-50-30+25+9- TIMELINE_OFFSET + VIDEO_TIMELINE_SIZE-50 + (AUDIO_TIMELINE_SIZE-20)/2,end_position_interval - start_position_interval - 1,AUDIO_TIMELINE_SIZE/2-1);

                String label = (*it)->sound->filename;
                File f(label);
                label = f.getFileName();
                g.setColour(Colour::fromRGB(50,50,50));
                int took_space = 32;
                if(took_space>end_position_interval - start_position_interval)
                {
                    took_space = 0;
                }
                if(took_space)
                    g.drawImageWithin(sound_image,start_position_interval+41,2+height_current - 75 - 30 + VIDEO_TIMELINE_SIZE - 1 - TIMELINE_OFFSET,36,AUDIO_TIMELINE_SIZE-4,RectanglePlacement::centred,false);



                g.drawFittedText(label + String(" [") + toolbox::format_duration((*it)->start) + String("  ; ") + toolbox::format_duration((*it)->end) + String("]"),took_space + start_position_interval + 50,height_current - 75 - 30 + VIDEO_TIMELINE_SIZE - 1 - TIMELINE_OFFSET,end_position_interval - start_position_interval - 15 - took_space,AUDIO_TIMELINE_SIZE,Justification::centredLeft,6);


            }
        }




        /*g.setColour(Colour::fromRGB(220,220,220));
        g.fillRect(41,height_current-50-30+25- TIMELINE_OFFSET + VIDEO_TIMELINE_SIZE-50,width_current-52-15,AUDIO_TIMELINE_SIZE/2-1);
        g.setColour(Colour::fromRGB(210,210,210));
        g.fillRect(41,height_current-50-30+25+9- TIMELINE_OFFSET + VIDEO_TIMELINE_SIZE-50 + (AUDIO_TIMELINE_SIZE-20)/2,width_current-52-15,AUDIO_TIMELINE_SIZE/2-1);*/
        //~List of sound intervals

        if(NeedDrawArrow())
            DrawArrow(g);

        DrawSlider(g);
    }


}
int MainComponent::GetArrowPosition(int arrow_position = -1)
{
    if(arrow_position<0)
        arrow_position = mouse_x;
    if(arrow_position<40)
        return 40;
    int max_y = getWidth()-25;
    if(arrow_position>max_y)
        return max_y;


    return arrow_position;

}
bool MainComponent::NeedDrawArrow()
{
    return (mouse_y>=getHeight()-125-30 - TIMELINE_OFFSET &&mouse_y<=getHeight()-75-30 - TIMELINE_OFFSET);
}

int MainComponent::GetCurrentPosition()
{
    if(timeline->current<timeline_position)
    {
        if(timeline_position==0)
            return 40;
        return -1;
    }
    double timeline_duration = (double)(getWidth()-65-1)/second_to_pixel;
    if(timeline->current>timeline_position + timeline_duration)
        return -1;


    return (int)round((double(getWidth()-65))*(timeline->current - timeline_position)/timeline_duration)+40;
}

double MainComponent::GetPositionSecond(int arrow_position = -1)
{
    if(arrow_position<0)
        arrow_position = mouse_x;

    double pos = GetArrowPosition(arrow_position) - 40;
    return pos / second_to_pixel + timeline_position;
}

void MainComponent::DrawSlider(Graphics& g)
{
    int position = GetCurrentPosition();
    if(position<0)
        return;
    int height_current = getHeight();
    g.setColour(Colour::fromRGB(150,100,100));
    g.drawLine(position,height_current - 75 - 30 - TIMELINE_OFFSET - 6,position,height_current - 75 - 30 - TIMELINE_OFFSET + VIDEO_TIMELINE_SIZE + AUDIO_TIMELINE_SIZE + 6,1.5);
}

void MainComponent::DrawArrow(Graphics& g)
{
    int position = GetArrowPosition();
    int height_current = getHeight();
    g.setColour(Colour::fromRGB(150,100,100));
    Line<float> line(position,height_current - 110-30- TIMELINE_OFFSET,position,height_current - 79-30 - TIMELINE_OFFSET);
    g.drawArrow(line,1,4,20);
}

void MainComponent::repaintSlider()
{
    repaint(0,getHeight()-200.0f - TIMELINE_OFFSET,getWidth(),200.0f + TIMELINE_OFFSET - 25.0f);
}

void MainComponent::mouseMove (const MouseEvent& e)
{
    mouse_x = e.x;
    mouse_y = e.y;

    mouseMoveReaction();

}

void MainComponent::mouseMoveReaction()
{
    if(!timeline_original)
    {
        Timeline::Interval *interval = 0;
        current_drag_x = mouse_x;
        current_drag_y = mouse_y;
        int found = -1;
        if(IsOverStream(0))
        {
            interval = timeline->FindIntervalBySecond(GetPositionSecond(mouse_x),0);
            found = 0;
        }
        else if(IsOverStream(1))
        {
            interval = timeline->FindIntervalBySecond(GetPositionSecond(mouse_x),1);
            found = 1;
        }

        current_drag_x = -1;

        for(vector<Timeline::Interval*>::iterator it = timeline->intervals_audio.begin(); it != timeline->intervals_audio.end(); it++)
        {
            if((*it)->selected)
                (*it)->color = Timeline::Interval::select;
            else
                (*it)->color = Timeline::Interval::usual;
        }

        for(vector<Timeline::Interval*>::iterator it = timeline->intervals_video.begin(); it != timeline->intervals_video.end(); it++)
        {
            if((*it)->selected)
                (*it)->color = Timeline::Interval::select;
            else
                (*it)->color = Timeline::Interval::usual;
        }

        if(interval && !interval->selected)
            interval->color = Timeline::Interval::over;
    }
    repaintSlider();
}

void MainComponent::mouseDown (const MouseEvent& e)
{
    mouse_x = e.x;
    mouse_y = e.y;

    if(e.mods.isLeftButtonDown())
    {
        if(NeedDrawArrow())
        {
            int position = GetArrowPosition();
            GotoSecondAndRead(GetPositionSecond(position));
        }
        if(!timeline_original)
        {

            Timeline::Interval *interval = 0;
            current_drag_x = mouse_x;
            current_drag_y = mouse_y;
            if(IsOverStream(0))
                interval = timeline->FindIntervalBySecond(GetPositionSecond(mouse_x),0);
            if(IsOverStream(1))
                interval = timeline->FindIntervalBySecond(GetPositionSecond(mouse_x),1);
            if(interval && interval->selected)
            {
                interval->selected = false;
                interval->color = Timeline::Interval::over;
                current_drag_x = -1;
                repaintSlider();
                return;
            }
            timeline->ResetIntervalColor();
            current_drag_x = -1;
            if(interval)
            {
                interval->color = Timeline::Interval::select;
                interval->selected = true;
            }
        }

    }
    else if(e.mods.isRightButtonDown())
    {
        PopupMenu context_menu;
        ApplicationCommandManager* const commandManager = mainWindow->commandManager;
        context_menu.addCommandItem(commandManager,commandSplit);
        context_menu.addCommandItem(commandManager,commandRemoveMovie,LABEL_DELETE_VIDEO_PART);
        context_menu.addCommandItem(commandManager,commandRemoveSpaces);
        context_menu.show();
    }

    repaintSlider();
}

const StringArray MainComponent::getMenuBarNames()
{
    const tchar* const names[] = { MENU_FILE,MENU_FRAME,MENU_VIDEO_PART, 0 };

    return StringArray ((const tchar**) names);
}

const PopupMenu MainComponent::getMenuForIndex (int menuIndex,
        const String& menuName)
{

    PopupMenu menu;

    ApplicationCommandManager* const commandManager = mainWindow->commandManager;

    switch (menuIndex)
    {
    case 0:
    {
        menu.addCommandItem(commandManager,commandOpen);
        PopupMenu sub_recent;
        recent.createPopupMenuItems(sub_recent,commandRecent,false,true);
        menu.addSubMenu(MENU_RECENT,sub_recent);
        menu.addCommandItem(commandManager,commandSave);
        menu.addSeparator();
        menu.addCommandItem(commandManager,commandPlay);
        menu.addCommandItem(commandManager,commandPause);
        menu.addCommandItem(commandManager,commandStop);
        menu.addSeparator();
        menu.addCommandItem(commandManager,commandShowTasks);
        menu.addSeparator();
        menu.addCommandItem(commandManager,commandRemoveSpaces);
        menu.addSeparator();
        menu.addCommandItem (commandManager, StandardApplicationCommandIDs::quit,MENU_QUIT);
    }
    break;
    case 1:
    {
        menu.addCommandItem(commandManager,commandSplit);
        menu.addCommandItem(commandManager,commandSaveFrame);
        PopupMenu sub_menu;
        sub_menu.addCommandItem(commandManager,commandNextSecond);
        sub_menu.addCommandItem(commandManager,commandNext5Frame);
        sub_menu.addCommandItem(commandManager,commandNextFrame);
        sub_menu.addCommandItem(commandManager,commandJump);
        sub_menu.addCommandItem(commandManager,commandPrevFrame);
        sub_menu.addCommandItem(commandManager,commandPrev5Frame);
        sub_menu.addCommandItem(commandManager,commandPrevSecond);
        menu.addSubMenu(MENU_JUMP,sub_menu);
    }
    break;
    case 2:
    {
        menu.addCommandItem(commandManager,commandRemoveMovie);
    }
    break;
    }

    return menu;
}

void MainComponent::menuItemSelected (int menuItemID, int topLevelMenuIndex)
{
    if(menuItemID>=commandRecent)
    {
        int num = menuItemID - commandRecent;
        if(num<recent.getNumFiles())
        {
            changeFileName(recent.getAllFilenames()[num]);
        }
    }
}

bool MainComponent::perform (const InvocationInfo& info)
{
    switch (info.commandID)
    {
    case commandRemoveMovie:
    {
        Timeline::Interval * interval = timeline->FindSelectedOrOver();
        if(interval)
        {
            StopVideo();
            if(timeline->intervals_video.size()==1 && encodeVideoWindow)
            {
                encodeVideoWindow->closeButtonPressed();
            }
            int interval_id = (interval->movie)?0:1;
            timeline->InsertIntervalIn(interval,interval_id,-2.0);
            repaint();
        }
    }
    break;
    case commandRemoveSpaces:
        {
            timeline->RemoveSpaces();
            sliderValueChanged(scale_timeline);
            repaint();
        }
    break;
    case commandShowTasks:
        {
            if(!tasks)
                tasks = new taskTab();
            tasks->add();
        }
    break;

    case commandSplit:
    {
        timeline->Split(0);
        timeline->Split(1);
        repaintSlider();
    }
    break;

    case commandOpen:
    {
        FileChooser fc (DIALOG_CHOOSE_FILE_TO_OPEN,File::getCurrentWorkingDirectory(),"*",true);
        if (fc.browseForFileToOpen())
        {
            StopVideo();
            File chosenFile = fc.getResult();
            changeFileName(chosenFile.getFullPathName());
        }
    }
    break;

    case commandPlay:
    {
        StartVideo();
    }
    break;

    case commandPause:
    {
        StopVideo();
    }
    break;

    case commandStop:
    {
        StopVideo();
        timeline->GotoSecondAndRead(0.0);
        repaint();
    }
    break;

    case commandNextFrame:
    {
        StopVideo();
        timeline->ReadAndDecodeFrame();
        repaint();
    }
    break;

    case commandPrevFrame:
    {
        StopVideo();

        timeline->GoBack(1);
        timeline->DecodeFrame(0);
        repaint();
    }
    break;

    case commandNext5Frame:
    {
        StopVideo();
        for(int i = 0; i<5; ++i)
        {
            timeline->SkipFrame();
        }
        timeline->DecodeFrame(0);
        repaint();

    }
    break;

    case commandPrev5Frame:
    {
        StopVideo();

        timeline->GoBack(5);
        timeline->DecodeFrame(0);
        repaint();
    }
    break;

    case commandNextSecond:
    {
        StopVideo();
        timeline->GotoSecondAndRead(timeline->current+1.0);
        repaint();

    }
    break;

    case commandPrevSecond:
    {
        StopVideo();
        timeline->GotoSecondAndRead(timeline->current-1.0);
        repaint();

    }
    break;

    case commandSave:
    {
        StopVideo();
        if(!encodeVideoWindow)
        {
            encodeVideoWindow = new encodeVideo(this);
        }else
        {
            encodeVideoWindow->add();
        }


    }


    break;

    case commandSaveFrame:
    {
        StopVideo();
        if(isVideoReady() && timeline->GetCurrentInterval(0))
        {

            FileChooser fc (DIALOG_CHOOSE_SCREENSHOT_TO_SAVE,timeline->GetCurrentInterval(0)->movie->filename + ".jpg","*.jpg",true);
            if (fc.browseForFileToSave(true))
            {
                File chosenFile = fc.getResult();
                bool can_save = true;
                if(chosenFile.existsAsFile())
                {
                    can_save = chosenFile.deleteFile();
                }

                can_save = can_save && chosenFile.hasWriteAccess();
                if(can_save)
                {
                    JPEGImageFormat *jpeg_format = new JPEGImageFormat();
                    OutputStream* stream = 0;

                    try
                    {

                        if(chosenFile.getFileName().toLowerCase().endsWith(".jpg"))
                        {
                            stream = chosenFile.createOutputStream();
                        }
                        else
                        {
                            File file_with_jpg_ext(chosenFile.getFullPathName() + ".jpg");
                            stream = file_with_jpg_ext.createOutputStream();
                        }

                        jpeg_format->writeImageToStream(*timeline->GetImage(),*stream);
                        if(stream)
                        {
                            delete stream;
                            stream = 0;
                        }
                        delete jpeg_format;
                        jpeg_format = 0;
                    }
                    catch(...)
                    {
                        can_save = false;
                        if(stream)delete stream;
                        if(jpeg_format)
                            delete jpeg_format;
                    }
                }
                if(can_save)
                    AlertWindow::showMessageBox(AlertWindow::InfoIcon,FILE_SAVED,chosenFile.getFullPathName());
                else
                    AlertWindow::showMessageBox(AlertWindow::WarningIcon,FILE_NOT_SAVED,chosenFile.getFullPathName());
            }

        }
    }
    break;

    case commandJump:
    {
        StopVideo();
        if(isVideoReady())
        {
            if(ask_jump_target)
            {
                delete ask_jump_target;
                ask_jump_target = 0;
            }
            ask_jump_target = new AskJumpDestanation(this);
        }

    }
    break;
    }
    return true;
}
ApplicationCommandTarget* MainComponent::getNextCommandTarget()
{
    return findFirstTargetParentComponent();
}

void MainComponent::getAllCommands (Array <CommandID>& commands)
{
    const CommandID ids[] = { commandOpen,
                              commandSave,
                              commandSaveFrame,
                              commandJump,
                              commandPlay,
                              commandPause,
                              commandPrevFrame,
                              commandNextFrame,
                              commandStop,
                              commandNext5Frame,
                              commandPrev5Frame,
                              commandNextSecond,
                              commandPrevSecond,
                              commandRemoveMovie,
                              commandSplit,
                              commandRemoveSpaces,
                              commandShowTasks
                            };

    commands.addArray (ids, numElementsInArray (ids));
}

bool MainComponent::isVideoReady ()
{
    return timeline->loaded;
}

void MainComponent::StopVideo()
{
    stopTimer();
    video_playing = false;
    miliseconds_start = -1;
    stop_sound();
}

void MainComponent::StartVideo()
{
    startTimer(1);
    video_playing = true;
    start_sound();
}

void MainComponent::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{

    switch (commandID)
    {
    case commandRemoveMovie:
        result.setInfo (LABEL_DELETE, LABEL_DELETE, MENU_VIDEO_PART, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.setActive(isVideoReady() && timeline->FindSelectedOrOver());
        break;
    case commandSplit:
        result.setInfo (LABEL_SPLIT, LABEL_SPLIT, MENU_FRAME, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.setActive(isVideoReady() && (timeline->current_interval_video || timeline->current_interval_audio) && !timeline->IsNearBoundary(0));
        break;
    case commandRemoveSpaces:
        result.setInfo (LABEL_REMOVE_SPACES, LABEL_REMOVE_SPACES, MENU_FILE, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.setActive(isVideoReady());
        break;
    case commandShowTasks:
        result.setInfo (MENU_SHOW_TASKS, MENU_SHOW_TASKS, MENU_FILE, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.addDefaultKeypress (T('T'), ModifierKeys::commandModifier);
        result.setActive(isVideoReady());
        break;
    case commandOpen:
        result.setInfo (MENU_FILE_OPEN, MENU_FILE_OPEN, MENU_FILE, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.addDefaultKeypress (T('O'), ModifierKeys::commandModifier);
        break;
    case commandSave:
        result.setInfo (MENU_FILE_SAVE, MENU_FILE_SAVE, MENU_FILE, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.addDefaultKeypress (T('S'), ModifierKeys::commandModifier);
        result.setActive(isVideoReady() && !timeline->IsEmpty());
        break;
    case commandSaveFrame:
        result.setInfo (MENU_SAVE_FRAME, MENU_SAVE_FRAME, MENU_FRAME, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.setActive(isVideoReady() && timeline->GetCurrentInterval(0));
        break;
    case commandJump:
        result.setInfo (LABEL_SPECIFIC_TIME, LABEL_SPECIFIC_TIME, MENU_FRAME, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.addDefaultKeypress (T('J'), ModifierKeys::commandModifier);
        result.setActive(isVideoReady());
        break;
    case commandPlay:
        result.setInfo (LABEL_PLAY, LABEL_PLAY, MENU_FILE, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.setActive(isVideoReady());
        break;
    case commandPause:
        result.setInfo (LABEL_PAUSE, LABEL_PAUSE, MENU_FILE, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.setActive(isVideoReady());
        break;
    case commandStop:
        result.setInfo (LABEL_STOP, LABEL_STOP, MENU_FILE, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.setActive(isVideoReady());
        break;
    case commandPrevFrame:
        result.setInfo (LABEL_PREV_FRAME, LABEL_PREV_FRAME, MENU_FRAME, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.setActive(isVideoReady());
        break;
    case commandNextFrame:
        result.setInfo (LABEL_NEXT_FRAME, LABEL_NEXT_FRAME, MENU_FRAME, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.setActive(isVideoReady());
        break;
    case commandPrev5Frame:
        result.setInfo (LABEL_PREV_FIVE_FRAME, LABEL_PREV_FIVE_FRAME, MENU_FRAME, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.setActive(isVideoReady());
        break;
    case commandNext5Frame:
        result.setInfo (LABEL_NEXT_FIVE_FRAME, LABEL_NEXT_FIVE_FRAME, MENU_FRAME, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.setActive(isVideoReady());
        break;
    case commandPrevSecond:
        result.setInfo (LABEL_PREV_SECOND, LABEL_PREV_SECOND, MENU_FRAME, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.addDefaultKeypress (T('P'), ModifierKeys::commandModifier);
        result.setActive(isVideoReady());
        break;
    case commandNextSecond:
        result.setInfo (LABEL_NEXT_SECOND, LABEL_NEXT_SECOND, MENU_FRAME, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.addDefaultKeypress (T('N'), ModifierKeys::commandModifier);
        result.setActive(isVideoReady());
        break;
    }


}

bool MainComponent::isInterestedInDragSource (const String& sourceDescription,Component* sourceComponent)
{
    return true;
}

void MainComponent::itemDropped (const String& sourceDescription,Component* sourceComponent,int x, int y)
{
    String description = getCurrentDragDescription();
     bool
    // video interval
    i = description.startsWith("i"),
    // sound interval
    s = description.startsWith("s"),
    // is over video stream
    iso0 = IsOverStream(0),
    // is over audio stream
    iso1 = IsOverStream(1),
    // movie from movie list
    m = description.startsWith("m");
    if((iso0 || i || iso1 || s) && timeline_original)
    {
        double pos = GetPositionSecond(x);
        int index = description.substring(1).getIntValue();

        if(m)
        {
            Movie * movie = timeline_original->movies[index];
            Timeline::Interval *current_interval = current_interval = new Timeline::Interval(movie,pos,movie->image_preview);
            timeline_original->InsertIntervalIn(current_interval,0);
        }
        else if(i || s)
        {
            int interval_id = (i)?0:1;
            vector<Timeline::Interval*> &intrvals_list = (i)?timeline_original->intervals_video:timeline_original->intervals_audio;

            Timeline::Interval *current_interval = intrvals_list[index];
            if(!IsOverStream(interval_id))
                pos = -2.0;

            if(pos>0.0)
            {
                pos -= dragIntervalOffset;
                if(pos<timeline_position)
                    pos = timeline_position;
            }
            timeline_original->InsertIntervalIn(current_interval,interval_id,pos);
        }
        current_drag_x = -1;
        delete timeline;
        if(timeline_original->intervals_video.size()==0 && encodeVideoWindow)
        {
            encodeVideoWindow->closeButtonPressed();
        }

        timeline = timeline_original;
        timeline_original = 0;
        sliderValueChanged(scale_timeline);
        mouse_x = x;
        mouse_y = y;
        mouseMoveReaction();
        ResizeViewport();
        repaint();
    }

}


bool MainComponent::IsOverStream(int interval_id)
{
   bool res;
   if(interval_id)
   //audio
        res = (current_drag_y <= getHeight() - 75 - 30 - TIMELINE_OFFSET + VIDEO_TIMELINE_SIZE + AUDIO_TIMELINE_SIZE && current_drag_y >= getHeight() - 75 - 30- TIMELINE_OFFSET + VIDEO_TIMELINE_SIZE && current_drag_x > 10 && current_drag_x<getWidth()-10);
   else
   //video
        res = (current_drag_y<= getHeight() - 75 - 30 - TIMELINE_OFFSET + VIDEO_TIMELINE_SIZE&&current_drag_y>=getHeight()-75 - 30- TIMELINE_OFFSET && current_drag_x>10 && current_drag_x<getWidth()-10);
   return res;
}


bool MainComponent::shouldDrawDragImageWhenOver()
{
    String description = getCurrentDragDescription();
    bool res = !IsOverStream(1) && !IsOverStream(0);
    if(description.startsWith("i") || description.startsWith("m"))
    {
        res = !IsOverStream(0);
    }else if(description.startsWith("s") || description.startsWith("a"))
    {
        res = !IsOverStream(1);
    }

    return res;
}

void MainComponent::itemDragMove (const String& sourceDescription,Component* sourceComponent,int x, int y)
{
    current_drag_x = x;
    current_drag_y = y;
    String desc = getCurrentDragDescription();
    bool
    // video interval
    i = desc.startsWith("i"),
    // sound interval
    s = desc.startsWith("s"),
    // is over video stream
    iso0 = IsOverStream(0),
    // is over audio stream
    iso1 = IsOverStream(1),
    // movie from movie list
    m = desc.startsWith("m");

    if(iso0 || i || iso1 || s)
    {
        int value = desc.substring(1).getIntValue();
        if(timeline_original)
        {
            delete timeline;
            timeline = 0;
        }
        else
            timeline_original = timeline;
        if(m)
        {
            Movie * movie = timeline_original->movies[value];
            Timeline::Interval *current_interval = new Timeline::Interval(movie,GetPositionSecond(current_drag_x),movie->image_preview);
            timeline = timeline_original->PreviewInsertIntervalIn(current_interval,0);
        }
        else if(i || s)
        {
            int interval_id = (i)?0:1;
            vector<Timeline::Interval*> &intrvals_list = (i)?timeline_original->intervals_video:timeline_original->intervals_audio;

            Timeline::Interval *current_interval = intrvals_list[value];
            if(timeline)
                dragIntervalOffset = (GetPositionSecond(current_drag_x) - current_interval->absolute_start);
            double pos = (!IsOverStream(interval_id))?-2.0:GetPositionSecond(current_drag_x);
            if(pos>0.0)
            {
                pos -= dragIntervalOffset;
                if(pos<timeline_position)
                    pos = timeline_position;
            }
            timeline = timeline_original->PreviewInsertIntervalIn(current_interval,interval_id,pos);
        }
    }
    else if(timeline_original)
    {
        delete timeline;
        timeline = timeline_original;
        timeline_original = 0;
    }

    repaintSlider();
}


void MainComponent::mouseDrag (const MouseEvent& e)
{
    if(!e.mods.isLeftButtonDown())return;
    if(!timeline_original)
    {
        current_drag_x = e.x;
        current_drag_y = e.y;
        if(IsOverStream(0))
        {
            double second = GetPositionSecond();
            int number = timeline->FindNumberIntervalBySecond(second,0);
            if(number>=0)
            {
                startDragging(String("i") + String(number),this,*timeline->intervals_video[number]->movie->image_preview);
                StopVideo();
            }
        }else if(IsOverStream(1))
        {
            double second = GetPositionSecond();
            int number = timeline->FindNumberIntervalBySecond(second,1);
            if(number>=0)
            {
                startDragging(String("s") + String(number),this,sound_image);
                StopVideo();
            }
        }
    }
}

void MainComponent::mouseExit(const MouseEvent& e)
{
    if(timeline_original)
        itemDropped(String::empty,0,e.x, e.y);
}

void MainComponent::GotoSecondAndRead(double second)
{
    timeline->GotoSecondAndRead(second);
    CallEventList(AfterChangePosition);
    ResizeViewport();
    repaint();
}

