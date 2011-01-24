#include "MainAppWindow.h"
#include "MainComponent.h"

#include "localization.h"
using namespace localization;
//==============================================================================
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


    MainComponent* contentComponent = new MainComponent (this);

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
    JUCEApplication::getInstance()->systemRequestedQuit();
}
