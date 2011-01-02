#include "AskJumpDestanation.h"
#include "MainComponent.h"
#include "localization.h"
#include "toolbox.h"
AskJumpDestanation::AskJumpDestanation(MainComponent* mainWindow_):AlertWindow(LABEL_CHOOSE_JUMP,LABEL_TOTAL_TIME + "  " + toolbox::format_duration(mainWindow_->timeline->duration),AlertWindow::QuestionIcon)
{
	mainWindow = mainWindow_;
	centreAroundComponent(mainWindow,0,0);

    toolbox::TimeVideo tv;
    tv.fromDouble(mainWindow_->timeline->current);

    String letters = T("0123456789");

    input_hours = new TextEditor();
    input_hours->setBounds(115,82,36,23);
    input_hours->setInputRestrictions(3,letters);
    input_hours->setSelectAllWhenFocused(true);
    input_hours->grabKeyboardFocus();
    addAndMakeVisible(input_hours);
    input_hours->setText(String(tv.hour));

    input_minutes = new TextEditor();
    input_minutes->setBounds(161,82,30,23);
    input_minutes->setInputRestrictions(2,letters);
    input_minutes->setSelectAllWhenFocused(true);
    input_minutes->grabKeyboardFocus();
    addAndMakeVisible(input_minutes);
    input_minutes->setText(String(tv.minute));

    input_seconds = new TextEditor();
    input_seconds->setBounds(201,82,30,23);
    input_seconds->setInputRestrictions(2,letters);
    input_seconds->setSelectAllWhenFocused(true);
    input_seconds->grabKeyboardFocus();
    addAndMakeVisible(input_seconds);
    input_seconds->setText(String(tv.second));

    input_microseconds = new TextEditor();
    input_microseconds->setBounds(241,82,36,23);
    input_microseconds->setInputRestrictions(3,letters);
    input_microseconds->setSelectAllWhenFocused(true);
    input_microseconds->grabKeyboardFocus();
    addAndMakeVisible(input_microseconds);
    input_microseconds->setText(String(tv.mini_second));

    okButton = new TextButton(LABEL_OK);
    okButton->setBounds(100,150,90,30);
    okButton->addButtonListener(this);
    addAndMakeVisible(okButton);

    cancelButton = new TextButton(LABEL_CANCEL);
    cancelButton->setBounds(210,150,90,30);
    cancelButton->addButtonListener(this);
    addAndMakeVisible(cancelButton);


    setVisible(true);
    addToDesktop(0);
    enterModalState();
    setSize(350,200);
}
void AskJumpDestanation::paint (Graphics& g)
{
    AlertWindow::paint(g);
    String twoDots = String(":");
    String dot = String(".");
    g.drawText(twoDots,151,82,10,23,Justification::centred,false);

    g.drawText(LABEL_HOURS,115,105,36,18,Justification::centred,false);
    g.drawText(LABEL_MINUTES,161,105,30,18,Justification::centred,false);
    g.drawText(LABEL_SECONDS,201,105,30,18,Justification::centred,false);
    g.drawText(LABEL_MINI_SECONDS,241,105,36,18,Justification::centred,false);

    g.drawText(twoDots,191,82,10,23,Justification::centred,false);
    g.drawText(dot,231,82,10,23,Justification::centredBottom,false);
}

AskJumpDestanation::~AskJumpDestanation()
{
	deleteAllChildren();

}

void AskJumpDestanation::buttonClicked (Button* button)
{
    if(button==okButton)
    {
       double input = (double)(input_hours->getText().getIntValue())*3600.0d + (double)(input_minutes->getText().getIntValue())*60.0d + (double)(input_seconds->getText().getIntValue()) + (double)(input_microseconds->getText().getIntValue())/1000.0d;
       if(fabs(mainWindow->timeline->current - input) > 0.01d)
       {
           mainWindow->timeline->GotoSecondAndRead(input);
           mainWindow->repaint();
       }
    }
    exitModalState(0);
    removeFromDesktop();
}
