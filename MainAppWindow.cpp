#include "config.h"
#include "MainAppWindow.h"


#include "localization.h"
using namespace localization;

MainAppWindow::MainAppWindow()
    :

    DocumentWindow (
        LABEL_PROGRAMM_CAPTION,
        Colours::whitesmoke,
        DocumentWindow::allButtons,
        true
    )
{

    commandManager = new ApplicationCommandManager();


    setResizeLimits (800, 600, 8192, 8192);

    setResizable (true, false);

    setTitleBarHeight (20);


    contentComponent = new MainComponent (this);

    commandManager->registerAllCommandsForTarget (contentComponent);
    commandManager->registerAllCommandsForTarget (JUCEApplication::getInstance());

    addKeyListener (commandManager->getKeyMappings());

    setContentComponent (contentComponent);

    contentComponent->setApplicationCommandManagerToWatch (commandManager);

    setMenuBar (contentComponent);
}

MainAppWindow::~MainAppWindow()
{
    delete commandManager;
}

void MainAppWindow::closeButtonPressed()
{
    if(!contentComponent->timeline || !contentComponent->timeline->loaded || AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,LABEL_ASK_QUIT,LABEL_ASK_QUIT,LABEL_YES,LABEL_NO))
        JUCEApplication::getInstance()->systemRequestedQuit();
}
