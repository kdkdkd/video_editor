#include "MainComponent.h"
#include "AskJumpDestanation.h"

void MainComponent::changeFileName(String new_filename)
{
    new_filename_cur = new_filename;
    movie_new = new Movie();
    bool loaded_local = false;

    loaded_local = movie_new->Load(new_filename_cur);
    if(loaded_local)
    {
        delete movie;
        movie = movie_new;
        filename = new_filename_cur;
        file_choosed = true;

        filename_label->setText(new_filename_cur,true);
        filename_label->setTooltip(new_filename_cur);
        filename_label->setVisible(true);
        movie->ReadAndDecodeFrame();
        SetVisibleButtons(true);
        repaint();

    }
    else
    {
        delete movie_new;
        AlertWindow::showMessageBox (AlertWindow::WarningIcon,CANT_LOAD_FILE,new_filename_cur);
    }
}

void MainComponent::SetVisibleButtons(bool visible)
{
    playButton->setVisible(visible);
    pauseButton->setVisible(visible);
    prevFrameButton->setVisible(visible);
    nextFrameButton->setVisible(visible);
    stopButton->setVisible(visible);

}

void  MainComponent::timerCallback()
{
    stopTimer();

    movie->ReadAndDecodeFrame();
    repaint();

    int spend = Time::getCurrentTime().toMilliseconds()-miliseconds_start;
    if(miliseconds_start<0)
        spend = 0;
    int need = 1000.0d / movie->fps;

    int timer = need - spend;

    if(timer<1)
        timer = 1;

    miliseconds_start = Time::getCurrentTime().toMilliseconds();
    startTimer(timer);

}

void MainComponent::buttonClicked (Button* button)
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


}
void MainComponent::initImageButton(String pic_name,DrawableButton*& button)
{
    button = new DrawableButton("",DrawableButton::ImageFitted);
    DrawableImage normal,over;
    Image* image = ImageCache::getFromFile(pic_name);
    normal.setImage (image, true);
    normal.setOpacity(0.8);
    over.setImage (image, true);
    button->setImages (&normal, &over, &normal);
    button->addButtonListener(this);
    addChildComponent(button);
}

MainComponent::MainComponent (MainAppWindow* mainWindow_)
{
    file_choosed = false;
    mainWindow = mainWindow_;

    filename_label = new Label ("","");
    addChildComponent(filename_label);
    av_register_all();


    initImageButton(String("pic\\play.png"),playButton);
    initImageButton(String("pic\\pause.png"),pauseButton);

    initImageButton(String("pic\\prev.png"),prevFrameButton);
    initImageButton(String("pic\\next.png"),nextFrameButton);
    initImageButton(String("pic\\stop.png"),stopButton);

    movie = new Movie();

    ask_jump_target = 0;

    video_playing = false;
    miliseconds_start = -1;
}

MainComponent::~MainComponent ()
{
    StopVideo();
    delete movie;
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
    filename_label->setBounds (0, height_current-20, width_current, 20);
    playButton->setBounds (10, height_current-195, 60, 65);
    pauseButton->setBounds (70, height_current-195, 60, 65);
    stopButton->setBounds (130, height_current-195, 60, 65);
    prevFrameButton->setBounds (width_current - 10 - 60 -60, height_current-195, 60, 65);
    nextFrameButton->setBounds (width_current - 10 - 60, height_current-195, 60, 65);
}

void MainComponent::paint (Graphics& g)
{
    if(isVideoReady())
    {
        g.setImageResamplingQuality(Graphics::lowResamplingQuality);

        int width_current = getWidth();
        int height_current = getHeight();

        int width_image = movie->image->getWidth();
        int height_image = movie->image->getHeight();

        float scalex = width_current/(float)width_image;
        float scaley = (height_current-210.0f)/(float)height_image;
        float scale = scalex;
        float deltax = 0.0f;
        float deltay = 0.0f;
        if(scaley<scalex)
        {
            scale = scaley;
            deltax = ((float)width_current - (float)width_image*scale)/2.0f;
        }
        else
        {
            deltay = ((float)height_current - 210.0f - (float)height_image*scale)/2.0f;
        }
        g.drawImageWithin(movie->image,deltax,deltay,width_image * scale,height_image * scale ,RectanglePlacement::centred,false);

        g.setColour(Colour::fromRGB(70,70,70));
        g.drawRect(25,height_current-75,width_current-50,50,1);

        g.drawVerticalLine(10,height_current-125,height_current-25);

        g.drawHorizontalLine(height_current-125,10,width_current-120);

        g.drawHorizontalLine(height_current-26,10,25);

        g.drawText(LABEL_TIME + String("   ") + toolbox::format_duration(movie->current) + String(" / ") + toolbox::format_duration(movie->duration),width_current-520,height_current-125,400,20,Justification::centredRight,true);

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
    return (int)round((double(getWidth()-50))*movie->current/(double)(movie->duration))+25;
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
    g.drawArrow(position,height_current - 110,position,height_current - 79,1,4,20);
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

        movie->GotoRatioAndRead(ratio);
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
        movie->GotoSecondAndRead(0.0);
        repaint();
    }
    break;

    case commandNextFrame:
    {
        StopVideo();
        movie->ReadAndDecodeFrame();
        repaint();
    }
    break;

    case commandPrevFrame:
    {
        StopVideo();

        movie->GoBack(1);
        movie->DecodeFrame();
        repaint();
    }
    break;

    case commandNext5Frame:
    {
        StopVideo();
        for(int i = 0; i<5; ++i)
        {
            movie->SkipFrame();
        }
        movie->DecodeFrame();
        repaint();

    }
    break;

    case commandPrev5Frame:
    {
        StopVideo();

        movie->GoBack(5);
        movie->DecodeFrame();
        repaint();
    }
    break;

    case commandNextSecond:
    {
        StopVideo();
        movie->GotoSecondAndRead(movie->current+1.0d);
        repaint();

    }
    break;

    case commandPrevSecond:
    {
        StopVideo();
        movie->GotoSecondAndRead(movie->current-1.0d);
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
            FileChooser fc (DIALOG_CHOOSE_SCREENSHOT_TO_SAVE,filename + ".jpg","*.jpg",true);
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
                        jpeg_format->writeImageToStream(*movie->image,*stream);
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
    return movie && movie->loaded ;
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
        result.setActive(isVideoReady());
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


