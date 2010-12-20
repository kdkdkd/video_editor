#include "MainComponent.h"
#include "AskJumpDestanation.h"

void MainComponent::changeFileName(String new_filename)
{
    stopThread (2000);

    new_filename_cur = new_filename;
    movie_new = new Movie();
    bool loaded_local = false;

    load_progress = 0.;
    filename_label->setVisible(true);
    filename_label->setText(new_filename_cur,true);
    filename_label->setTooltip(new_filename_cur);



    repaint();

    loaded_local = movie_new->Open(new_filename_cur);
    if(loaded_local)
    {
        startThread(6);
        //run();
        last_estimation = "";
        operation_start = Time::getCurrentTime().toMilliseconds();
        operation_estimate = operation_start;
        progress->setVisible(true);
    }
    else
    {
        load_progress = -1.;
        repaint();
        delete movie_new;
        progress->setVisible(false);
        AlertWindow::showMessageBox (AlertWindow::WarningIcon,CANT_LOAD_FILE,new_filename_cur);
        if(!loaded_something)
        {
            filename_label->setVisible(false);
        }

    }
}


void MainComponent::run()
{
    double prep_res = movie_new->Prepare();

    while (! threadShouldExit() && prep_res>=0.0)
    {

        prep_res = movie_new->Prepare();

        const MessageManagerLock mml (Thread::getCurrentThread());

        if (! mml.lockWasGained())
            return;


        int64 time_now = Time::getCurrentTime().toMilliseconds();
        int64 delta_time = time_now - operation_start;
        int64 delta_time_estimate = time_now - operation_estimate;
        if(prep_res>0.05 && delta_time_estimate>1000)
        {
            last_estimation = LABEL_ESTIMATE + "  " + toolbox::format_duration((double)(delta_time/1000)*(1.0 - prep_res)/prep_res);
            operation_estimate = time_now;
            estimate_label->setVisible(true);
        }
        estimate_label->setText(last_estimation,true);

        load_progress = prep_res;
        repaint();

    }

    const MessageManagerLock mml (Thread::getCurrentThread());
    estimate_label->setVisible(false);
    progress->setVisible(false);
    bool loaded_local = prep_res == -1;
    if(loaded_local)
    {
        loaded_local = movie_new->GetInfo();
    }

    load_progress = -1.;
    filename_label->setVisible(true);
    repaint();

    if(loaded_local)
    {

        delete movie;
        loaded_something=true;
        movie = movie_new;
        filename = new_filename_cur;
        file_choosed = true;

        filename_label->setText(new_filename_cur,true);
        filename_label->setTooltip(new_filename_cur);
        filename_label->setVisible(true);

        movie->ReadFrame();
        current_frame = 0;
        repaint();
    }
    else
    {
        delete movie_new;
        AlertWindow::showMessageBox (AlertWindow::WarningIcon,CANT_LOAD_FILE,new_filename_cur);
        if(!loaded_something)
        {
            filename_label->setVisible(false);
        }
    }

}

void MainComponent::buttonClicked (Button* button)
{

}


MainComponent::MainComponent (MainAppWindow* mainWindow_):Thread("worker2")
{
    file_choosed = false;
    mainWindow = mainWindow_;

    progress = new ProgressBar(load_progress);
    addChildComponent(progress);

    filename_label = new Label ("","");
    estimate_label = new Label ("","");

    addChildComponent(estimate_label);

    loaded_something = false;
    addChildComponent(filename_label);
    av_register_all();
    movie = new Movie();
    current_frame = 0;
    load_progress = -1.;
    ask_jump_target = 0;
}

MainComponent::~MainComponent ()
{
    delete movie;
    if(ask_jump_target)
    {
        delete ask_jump_target;
        ask_jump_target = 0;
    }
    stopThread (2000);
    deleteAllChildren();
}


void MainComponent::resized ()
{

    int width_current = getWidth();
    int height_current = getHeight();
    filename_label->setBounds (0, height_current-20, width_current, 20);
    progress->setBounds (10, height_current/2-30, width_current-20, 60);
    estimate_label->setBounds (10, height_current/2+35, width_current-20, 12);
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
        float scaley = (height_current-220.0f)/(float)height_image;
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
            deltay = ((float)height_current - 220.0f - (float)height_image*scale)/2.0f;
        }
        g.drawImageWithin(movie->image,deltax,deltay,width_image * scale,height_image * scale ,RectanglePlacement::stretchToFit,false);

        g.setColour(Colour::fromRGB(70,70,70));
        g.drawRect(25,height_current-105,width_current-50,30,1);

        g.drawVerticalLine(10,height_current-155,height_current-105+30);
        g.drawVerticalLine(width_current - 11,height_current-75-30,height_current-25);
        g.drawHorizontalLine(height_current-155,10,width_current-120);
        g.drawHorizontalLine(height_current-25,220,width_current-10);

        g.drawHorizontalLine(height_current-105+29,10,25);
        g.drawHorizontalLine(height_current-105,width_current-25,width_current-10);


        g.drawText(LABEL_TIME + String("   ") + toolbox::format_duration((int)(movie->FrameToDuration(current_frame))) + String(" / ") + toolbox::format_duration((int)(movie->duration)),width_current-320,height_current-155,200,20,Justification::centredRight,true);
        g.drawText(LABEL_FRAMES + String("   ") + String(current_frame+1) + String(" / ")+ String(movie->total_frames),220,height_current-45,200,20,Justification::centredLeft,true);


        g.setColour(Colour::fromRGB(200,200,250));
        g.fillRect(26,height_current-104,width_current-52,14);

        g.setColour(Colour::fromRGB(180,180,230));
        g.fillRect(26,height_current-104+14,width_current-52,14);


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


    return GetPositionByFrame(GetFrameFromPosition(mouse_x));

}
bool MainComponent::NeedDrawArrow()
{
    return ((mouse_y>=getHeight()-155&&mouse_y<=getHeight()-105)||(mouse_y>=getHeight()-105+30&&mouse_y<=getHeight()-25));
}

int MainComponent::GetFrameFromPosition(int position)
{
    double ratio = (double)(position-25)/(double)(getWidth()-50);
    return (int)round(ratio*(double)(movie->total_frames-1));
}

int MainComponent::GetPositionByFrame(int frame)
{
    return (int)round((double(getWidth()-50))*(double) frame/(double)(movie->total_frames-1))+25;
}

void MainComponent::DrawSlider(Graphics& g)
{
    int position = GetPositionByFrame(current_frame);
    int height_current = getHeight();
    g.setColour(Colour::fromRGB(255,255,255));
    g.fillRoundedRectangle(position-3,height_current-105-5,6,40,4);
    g.setColour(Colour::fromRGB(150,100,100));
    g.drawRoundedRectangle(position-3,height_current-105-5,6,40,4,1.5);
}

void MainComponent::DrawArrow(Graphics& g)
{
    int position = GetArrowPosition();
    int height_current = getHeight();
    g.setColour(Colour::fromRGB(150,100,100));
    g.drawArrow(position,height_current - 140,position,height_current - 105-4,1,4,20);
    g.drawArrow(position,height_current - 105+30+35,position,height_current - 105+30+4,1,4,20);
}

void MainComponent::repaintSlider()
{
    repaint(0,getHeight()-220.0f,getWidth(),200.0f);
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
        int new_frame = GetFrameFromPosition(GetArrowPosition());
        movie->GotoFrameAndRead(new_frame);
        {
            current_frame = new_frame;
            repaint();
            return;
        }
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
    //AlertWindow::showMessageBox(AlertWindow::QuestionIcon,MENU_FILE_OPEN,MENU_FILE_OPEN);
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
            FileChooser fc (DIALOG_CHOOSE_SCREENSHOT_TO_SAVE,filename + "_frame_" + String(current_frame) + ".jpg","*.jpg",true);
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
        /*AlertWindow *ask_where = new AlertWindow(LABEL_CHOOSE_JUMP,LABEL_FROM + " 1 " + LABEL_TO + " " + String(movie->total_frames),AlertWindow::QuestionIcon);
        ask_where->centreAroundComponent (this,0,0);
        ask_where->addButton(LABEL_OK,0);
        ask_where->addButton(LABEL_CANCEL,-1);

        ask_where->setVisible(true);
        ask_where->addToDesktop(0);*/

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
    // this will return the next parent component that is an ApplicationCommandTarget (in this
    // case, there probably isn't one, but it's best to use this method in your own apps).
    return findFirstTargetParentComponent();
}

void MainComponent::getAllCommands (Array <CommandID>& commands)
{
    // this returns the set of all commands that this target can perform..
    const CommandID ids[] = { commandOpen,
                              commandSave,
                              commandEncode,
                              commandSaveFrame,
                              commandJump

                            };

    commands.addArray (ids, numElementsInArray (ids));
}

bool MainComponent::isVideoReady ()
{
    return movie&&movie->loaded && load_progress<0.;
}

void MainComponent::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{
    const String generalCategory (T("General"));
    const String demosCategory (T("Demos"));

    switch (commandID)
    {
    case commandOpen:
        result.setInfo (MENU_FILE_OPEN, MENU_FILE_OPEN, MENU_FILE, 0);
        result.addDefaultKeypress (T('O'), ModifierKeys::commandModifier);
        //result.setActive(load_progress<0.);
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
    }
}


