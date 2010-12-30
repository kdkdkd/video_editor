#include "AskJumpDestanation.h"
#include "MainComponent.h"
#include "localization.h"
AskJumpDestanation::AskJumpDestanation(MainComponent* mainWindow_):AlertWindow(LABEL_CHOOSE_JUMP,LABEL_FROM + " 1 " + LABEL_TO + " 2 ",AlertWindow::QuestionIcon)
{
	mainWindow = mainWindow_;
	centreAroundComponent(mainWindow,0,0);
    addButton(LABEL_OK,1);
    addButton(LABEL_CANCEL,0);

    input = new TextEditor("input");
    input->setBounds(90,62,100,25);

    /*int number = mainWindow_->movie->total_frames;
    input->setInputRestrictions(floor( log10( abs( number != 0 ? number : 1 ) ) ) + 1,"0123456789");

    input->setText(String(mainWindow_->current_frame+1));*/

    input->setSelectAllWhenFocused(true);
    input->grabKeyboardFocus();

    addAndMakeVisible(input);
    setVisible(true);
    addToDesktop(0);
    enterModalState();
}

AskJumpDestanation::~AskJumpDestanation()
{
	deleteAllChildren();

}

void AskJumpDestanation::buttonClicked (Button* button)
{
    /*if(button->getCommandID())
    {
       String input_text = input->getText();
       int input_int = input_text.getIntValue();
       input_int--;
       int total_index = mainWindow->movie->total_frames-1;
       if(input_int<0)input_int=0;
       else if(input_int>total_index) input_int = total_index;

       if(mainWindow->current_frame != input_int)
       {

           mainWindow->movie->GotoFrameAndRead(input_int);
           mainWindow->current_frame = input_int;
           mainWindow->repaint();
       }
    }*/
    exitModalState(0);
    removeFromDesktop();
}
