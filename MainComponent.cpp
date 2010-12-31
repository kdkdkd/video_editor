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

    }else
    {
        delete movie_new;
        AlertWindow::showMessageBox (AlertWindow::WarningIcon,CANT_LOAD_FILE,new_filename_cur);
    }
}

void MainComponent::SetVisibleButtons(bool visible)
{
    playButton->setVisible(visible);
    pauseButton->setVisible(visible);

}

void  MainComponent::timerCallback()
{
    movie->ReadAndDecodeFrame();
    repaint();
}




MainComponent::MainComponent (MainAppWindow* mainWindow_)
{
    file_choosed = false;
    mainWindow = mainWindow_;

    filename_label = new Label ("","");
    addChildComponent(filename_label);
    av_register_all();


    ApplicationCommandManager* const commandManager = mainWindow->commandManager;

    playButton = new DrawableButton("",DrawableButton::ImageFitted);
    DrawableImage normal,over;
    Image* play_image = ImageCache::getFromFile(String("pic\\play.png"));
    normal.setImage (play_image, true);
    normal.setOpacity(0.8);
    over.setImage (play_image, true);
    playButton->setImages (&normal, &over, &normal);
    playButton->setCommandToTrigger(commandManager,commandPlay,true);
    addChildComponent(playButton);

    pauseButton = new DrawableButton("",DrawableButton::ImageFitted);
    DrawableImage normal_pause,over_pause;
    Image* pause_image = ImageCache::getFromFile(String("pic\\pause.png"));
    normal_pause.setImage (pause_image, true);
    normal_pause.setOpacity(0.8);
    over_pause.setImage (pause_image, true);
    pauseButton->setImages (&normal_pause, &over_pause, &normal_pause);
    pauseButton->setCommandToTrigger(commandManager,commandPause,true);
    addChildComponent(pauseButton);


    movie = new Movie();

    ask_jump_target = 0;
}

MainComponent::~MainComponent ()
{
    stopTimer();
    delete movie;
    if(ask_jump_target)
    {
        delete ask_jump_target;
        ask_jump_target = 0;
    }
    deleteAllChildren();
    delete mainWindow->commandManager;
}


void MainComponent::resized ()
{
    int width_current = getWidth();
    int height_current = getHeight();
    filename_label->setBounds (0, height_current-20, width_current, 20);
    playButton->setBounds (10, height_current-195, 60, 65);
    pauseButton->setBounds (70, height_current-195, 60, 65);
}

void MainComponent::paint (Graphics& g)
{
    if(isVideoReady())
    {

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
        g.drawImageWithin(movie->image,deltax,deltay,width_image * scale,height_image * scale ,RectanglePlacement::stretchToFit,false);

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

        movie->GotoAndRead(ratio);
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
        menu.addSeparator();
        menu.addCommandItem(commandManager,commandEncode);
        menu.addSeparator();
        menu.addCommandItem(commandManager,commandPlay);
        menu.addCommandItem(commandManager,commandPause);
        menu.addSeparator();
        menu.addCommandItem (commandManager, StandardApplicationCommandIDs::quit,MENU_QUIT);
    }
    break;
    case 1:
    {
        menu.addCommandItem(commandManager,commandJump);
        menu.addCommandItem(commandManager,commandSaveFrame);
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
            File chosenFile = fc.getResult();
            changeFileName(chosenFile.getFullPathName());
        }
    }
    break;

    case commandPlay:
    {
        startTimer(20);
    }
    break;

    case commandPause:
    {
        stopTimer();
    }
    break;

    case commandSave:
    {

    }
    break;

    case commandEncode:
        break;

    case commandSaveFrame:
    {
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
                        }else
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
                              commandPause

                            };

    commands.addArray (ids, numElementsInArray (ids));
}

bool MainComponent::isVideoReady ()
{
    return movie && movie->loaded ;
}

void MainComponent::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{

    switch (commandID)
    {
    case commandOpen:
        result.setInfo (MENU_FILE_OPEN, MENU_FILE_OPEN, MENU_FILE, 0);
        result.addDefaultKeypress (T('O'), ModifierKeys::commandModifier);
        break;
    case commandSave:
        result.setInfo (MENU_FILE_SAVE, MENU_FILE_SAVE, MENU_FILE, 0);
        result.addDefaultKeypress (T('S'), ModifierKeys::commandModifier);
        result.setActive(false);
        break;
    case commandEncode:
        result.setInfo (MENU_FILE_ENCODE, MENU_FILE_ENCODE, MENU_FILE, 0);
        result.addDefaultKeypress (T('E'), ModifierKeys::commandModifier);
        result.setActive(false);
        break;
    case commandSaveFrame:
        result.setInfo (MENU_SAVE_FRAME, MENU_SAVE_FRAME, MENU_FRAME, 0);
        result.setActive(isVideoReady());
        break;
    case commandJump:
        result.setInfo (MENU_JUMP, MENU_JUMP, MENU_FRAME, 0);
        result.addDefaultKeypress (T('J'), ModifierKeys::commandModifier);
        result.setActive(isVideoReady());
        break;
    case commandPlay:
        result.setInfo (LABEL_PLAY, LABEL_PLAY, MENU_FILE, 0);
        result.setActive(isVideoReady());
        break;
    case commandPause:
        result.setInfo (LABEL_PAUSE, LABEL_PAUSE, MENU_FILE, 0);
        result.setActive(isVideoReady());
        break;
    }
}


