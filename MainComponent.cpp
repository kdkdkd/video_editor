#include "MainComponent.h"
#include "PopupWindow.h"
#include "AskJumpDestanation.h"
#include "DrawableButtonAndDrag.h"


void MainComponent::changeFileName(String new_filename)
{
    bool loaded_local = false;

    loaded_local = timeline->Load(new_filename);
    if(loaded_local)
    {
        SetVisibleButtons(true);
        AddMovieToList(timeline->GetCurrentMovie());
        ResizeViewport();
        repaint();
    }
    else
    {
        AlertWindow::showMessageBox (AlertWindow::WarningIcon,CANT_LOAD_FILE,new_filename);
    }
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
    prevFrameButton->setVisible(visible);
    nextFrameButton->setVisible(visible);
    stopButton->setVisible(visible);
    movies_list->setVisible(visible);
}

void MainComponent::ResizeViewport()
{
    int height_current = getHeight();
    int movies_border = 300;
    if(isVideoReady())
    {
        movies_border = GetMoviesBorder();
    }
    movies_list->setBounds(13,10 + 18,movies_border-15,height_current - 230 - 10 - 18 - 1);

}

void  MainComponent::timerCallback()
{
    stopTimer();

    timeline->ReadAndDecodeFrame();
    repaint();

    int spend = Time::getCurrentTime().toMilliseconds()-miliseconds_start;
    if(miliseconds_start<0)
        spend = 0;
    int need = 1000.0d / timeline->GetCurrentMovie()->fps;

    int timer = need - spend;

    if(timer<1)
        timer = 1;

    miliseconds_start = Time::getCurrentTime().toMilliseconds();
    startTimer(timer);

}

void MainComponent::buttonClickedWithMods (Button* button, const ModifierKeys&  e)
{


    ApplicationCommandManager* const commandManager = mainWindow->commandManager;
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

        if(e.isRightButtonDown())
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
                toolbox::show_info_popup(LABEL_INFO,movie->GetMovieInfo(),this);
                break;
            case 1001:
                Component *viewed = button->getParentComponent();
                for(int i = 0; i<viewed->getNumChildComponents(); ++i)
                {
                    Component *child = viewed->getChildComponent(i);
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

MainComponent::MainComponent (MainAppWindow* mainWindow_)
{
    mainWindow = mainWindow_;

    av_register_all();

    initImageButton(String("pic\\play.png"),playButton);
    initImageButton(String("pic\\pause.png"),pauseButton);

    initImageButton(String("pic\\prev.png"),prevFrameButton);
    initImageButton(String("pic\\next.png"),nextFrameButton);
    initImageButton(String("pic\\stop.png"),stopButton);

    timeline = new Timeline();

    movies_list = new ContainerBox("movies_list");
    addChildComponent(movies_list);

    ask_jump_target = 0;

    video_playing = false;
    miliseconds_start = -1;
}

MainComponent::~MainComponent ()
{
    StopVideo();
    delete timeline;
    if(ask_jump_target)
    {
        delete ask_jump_target;
        ask_jump_target = 0;
    }
    deleteAllChildren();

}


void MainComponent::resized ()
{
    int width_current = getWidth();
    int height_current = getHeight();
    playButton->setBounds (10, height_current-195-25, 60, 65);
    pauseButton->setBounds (70, height_current-195-25, 60, 65);
    stopButton->setBounds (130, height_current-195-25, 60, 65);
    prevFrameButton->setBounds (width_current - 10 - 60 -60, height_current-195-25, 60, 65);
    nextFrameButton->setBounds (width_current - 10 - 60, height_current-195-25, 60, 65);
    ResizeViewport();


}

int MainComponent::GetMoviesBorder()
{
    int width_current = getWidth();
    int height_current = getHeight();

    int width_image = timeline->GetImage()->getWidth();
    int height_image = timeline->GetImage()->getHeight();

    int res = 300;
    float scalex = (width_current-310.0f)/(float)width_image;
    float scaley = (height_current-230.0f)/(float)height_image;
    if(scaley<scalex)
    {
        res += width_current - 310 - (int)(width_image*scaley)-10;
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

        float scalex = (width_current-310.0f)/(float)width_image;
        float scaley = (height_current-230.0f)/(float)height_image;
        float scale = scalex;
        float deltax = 305.0f;
        float deltay = 0.0f;
        if(scaley<scalex)
        {
            scale = scaley;
            deltax += ((float)width_current - 310.0f - (float)width_image*scale) - 5.0f;
        }
        else
        {
            deltay += ((float)height_current - 230.0f - (float)height_image*scale)/2.0f;
        }

        g.drawImageWithin(*(timeline->GetImage()),deltax,deltay,(width_image * scale),(height_image * scale) ,RectanglePlacement::centred,false);

        g.setColour(Colour::fromRGB(70,70,70));
        g.drawRect(25,height_current-75,width_current-50,50,1);

        g.drawVerticalLine(10,height_current-125,height_current-25);

        g.drawHorizontalLine(height_current-125,10,width_current-120);

        g.drawHorizontalLine(height_current-26,10,25);

        g.drawText(LABEL_TIME + String("   ") + toolbox::format_duration(timeline->current) + String(" / ") + toolbox::format_duration(timeline->duration),width_current-520,height_current-125,400,20,Justification::centredRight,true);

        Font f = g.getCurrentFont();
        Font f_copy = f;
        f.setItalic(true);
        f.setHeight(18);
        g.setFont(f);
        int text_height = f.getHeight();
        int text_width = f.getStringWidth(LABEL_MOVIES);
        g.drawText(LABEL_MOVIES,30,10,text_width,text_height,Justification::centred,true);
        int end_height = GetMoviesBorder();
        g.drawHorizontalLine(10 + text_height/2,30 + text_width + 3,end_height);
        g.drawHorizontalLine(10 + text_height/2,10,27);
        g.drawHorizontalLine(height_current - 230,10,end_height);
        g.drawVerticalLine(10,10 +text_height/2,height_current - 230);
        g.drawVerticalLine(end_height,10 +text_height/2,height_current - 230);
        g.setFont(f_copy);


        g.setColour(Colour::fromRGB(200,200,250));
        g.fillRect(26,height_current-74,width_current-52,24);

        g.setColour(Colour::fromRGB(180,180,230));
        g.fillRect(26,height_current-50,width_current-52,24);


        if(NeedDrawArrow())
            DrawArrow(g);

        DrawSlider(g);
    }


}
int MainComponent::GetArrowPosition()
{
    if(mouse_x<25)
        return 25;
    int max_y = getWidth()-25;
    if(mouse_x>max_y)
        return max_y;


    return mouse_x;

}
bool MainComponent::NeedDrawArrow()
{
    return (mouse_y>=getHeight()-125&&mouse_y<=getHeight()-75);
}

int MainComponent::GetCurrentPosition()
{
    return (int)round((double(getWidth()-50))*timeline->current/(double)(timeline->duration))+25;
}

void MainComponent::DrawSlider(Graphics& g)
{
    int position = GetCurrentPosition();
    int height_current = getHeight();
    g.setColour(Colour::fromRGB(255,255,255));
    g.fillRoundedRectangle(position-3,height_current-80,6,60,4);
    g.setColour(Colour::fromRGB(150,100,100));
    g.drawRoundedRectangle(position-3,height_current-80,6,60,4,1.5);
}

void MainComponent::DrawArrow(Graphics& g)
{
    int position = GetArrowPosition();
    int height_current = getHeight();
    g.setColour(Colour::fromRGB(150,100,100));
    Line<float> line(position,height_current - 110,position,height_current - 79);
    g.drawArrow(line,1,4,20);
}

void MainComponent::repaintSlider()
{
    repaint(0,getHeight()-170.0f,getWidth(),170.0f);
}

void MainComponent::mouseMove (const MouseEvent& e)
{
    mouse_x = e.x;
    mouse_y = e.y;

    repaintSlider();
}

void MainComponent::mouseDown (const MouseEvent& e)
{
    mouse_x = e.x;
    mouse_y = e.y;
    if(NeedDrawArrow())
    {
        int position = GetArrowPosition();
        double ratio = (double)(position-25)/(double)(getWidth()-50);

        timeline->GotoRatioAndRead(ratio);
        repaint();
        return;
    }
    repaintSlider();
}

const StringArray MainComponent::getMenuBarNames()
{
    const tchar* const names[] = { MENU_FILE,MENU_FRAME, 0 };

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
        menu.addCommandItem(commandManager,commandSave);
        menu.addCommandItem(commandManager,commandEncode);
        menu.addSeparator();
        menu.addCommandItem(commandManager,commandPlay);
        menu.addCommandItem(commandManager,commandPause);
        menu.addCommandItem(commandManager,commandStop);
        menu.addSeparator();
        menu.addCommandItem (commandManager, StandardApplicationCommandIDs::quit,MENU_QUIT);
    }
    break;
    case 1:
    {
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
    }

    return menu;
}

void MainComponent::menuItemSelected (int menuItemID, int topLevelMenuIndex)
{

}

bool MainComponent::perform (const InvocationInfo& info)
{
    switch (info.commandID)
    {
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
        timeline->DecodeFrame();
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
        timeline->DecodeFrame();
        repaint();

    }
    break;

    case commandPrev5Frame:
    {
        StopVideo();

        timeline->GoBack(5);
        timeline->DecodeFrame();
        repaint();
    }
    break;

    case commandNextSecond:
    {
        StopVideo();
        timeline->GotoSecondAndRead(timeline->current+1.0d);
        repaint();

    }
    break;

    case commandPrevSecond:
    {
        StopVideo();
        timeline->GotoSecondAndRead(timeline->current-1.0d);
        repaint();

    }
    break;

    case commandSave:
    {
        StopVideo();
    }


    break;

    case commandEncode:
    {
        StopVideo();
    }
    break;

    case commandSaveFrame:
    {
        StopVideo();
        if(isVideoReady())
        {
            FileChooser fc (DIALOG_CHOOSE_SCREENSHOT_TO_SAVE,timeline->GetCurrentMovie()->filename + ".jpg","*.jpg",true);
            if (fc.browseForFileToSave(true))
            {
                File chosenFile = fc.getResult();
                bool can_save = true;
                if(chosenFile.exists())
                {
                    can_save = chosenFile.deleteFile();
                }


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
                        if(stream)delete stream;
                        delete jpeg_format;
                    }
                    catch(...)
                    {
                        can_save = false;
                        if(stream)delete stream;
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
                              commandEncode,
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
                              commandPrevSecond
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
}

void MainComponent::StartVideo()
{
    startTimer(1);
    video_playing = true;
}

void MainComponent::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{

    switch (commandID)
    {
    case commandOpen:
        result.setInfo (MENU_FILE_OPEN, MENU_FILE_OPEN, MENU_FILE, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.addDefaultKeypress (T('O'), ModifierKeys::commandModifier);
        break;
    case commandSave:
        result.setInfo (MENU_FILE_SAVE, MENU_FILE_SAVE, MENU_FILE, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.addDefaultKeypress (T('S'), ModifierKeys::commandModifier);
        result.setActive(false);
        break;
    case commandEncode:
        result.setInfo (MENU_FILE_ENCODE, MENU_FILE_ENCODE, MENU_FILE, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.addDefaultKeypress (T('E'), ModifierKeys::commandModifier);
        result.setActive(false);
        break;
    case commandSaveFrame:
        result.setInfo (MENU_SAVE_FRAME, MENU_SAVE_FRAME, MENU_FRAME, ApplicationCommandInfo::dontTriggerVisualFeedback);
        result.setActive(isVideoReady());
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


