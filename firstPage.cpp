#include "firstPage.h"


class SpecialButton : public Button
{
    public:
    SpecialButton(const String& buttonName):Button(buttonName)
    {
        setMouseCursor (MouseCursor::PointingHandCursor);
    }

    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown)
    {
        const Colour textColour (findColour(0x1001f00));
        g.setColour ((isMouseOverButton) ? textColour.darker ((isButtonDown) ? 3.5f : 1.6f) : textColour);
        g.drawText (getButtonText(), 2, 0, getWidth() - 2, getHeight(), Justification::centredLeft, true);
    }

};

void firstPage::resized()
{
    int size = recent_list.size();
    int width = getWidth();
    for(int i = 0;i<size;++i)
    {
        recent_list[i]->setBounds(width/2 - 100, i*20 + 150,300,20);
    }
}

void firstPage::paint(Graphics &g)
{
    //g.fillAll(Colours::aquamarine);
}

firstPage::firstPage(MainComponent* main)
{
    this->main = main;
    StringArray files = main->recent.getAllFilenames();
    int size = files.size();

    for(int i = 0;i<size;++i)
    {

        SpecialButton * new_button = new SpecialButton(files[i]);
        new_button->setButtonText(File(files[i]).getFileName());
        addAndMakeVisible(new_button);
        new_button->addListener(this);
        recent_list.add(new_button);
    }
}
void firstPage::buttonClicked(Button* button)
{
    main->changeFileName(button->getName());
}

firstPage::~firstPage()
{
    deleteAllChildren();
}

