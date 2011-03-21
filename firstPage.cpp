#include "firstPage.h"

Image border;

class SpecialButton : public Button
{
    public:
    SpecialButton(const String& buttonName):Button(buttonName)
    {
        setMouseCursor (MouseCursor::PointingHandCursor);
    }

    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown)
    {
        const Colour textColour(findColour(0x1001f00));
        g.setColour(Colours::whitesmoke);
        g.drawImage(border,0,0,getWidth(),getHeight(),0,0,95,31,true);
        g.setColour((isMouseOverButton) ? textColour.darker ((isButtonDown) ? 3.5f : 1.6f) : textColour);


        g.drawText(getButtonText(), 30, 0, getWidth() - 62, getHeight(), Justification::centredLeft, true);
    }

};

void firstPage::resized()
{
    int size = recent_list.size();
    int width = getWidth();
    int delta = getHeight()/2 - size*20;
    if(delta<170)
        delta = 170;
    for(int i = 0;i<size;++i)
    {
        recent_list[i]->setBounds(width/2 - 150, i*35 + delta,300,35);
    }
}

void firstPage::paint(Graphics &g)
{
    g.drawImage(logo,0,0,getWidth(),getHeight(),0,0,300,179,false);
}

firstPage::firstPage(MainComponent* main)
{
    this->main = main;
    StringArray files = main->recent.getAllFilenames();
    int size = files.size();

    for(int i = 0;i<size;++i)
    {
        if(File(files[i]).exists())
        {
            SpecialButton * new_button = new SpecialButton(files[i]);
            new_button->setButtonText(File(files[i]).getFileName());
            addAndMakeVisible(new_button);
            new_button->addListener(this);
            recent_list.add(new_button);
        }
    }
    logo = ImageCache::getFromFile(String("../pic/logo.png"));
    border = ImageCache::getFromFile(String("../pic/border.png"));

}
void firstPage::buttonClicked(Button* button)
{
    main->changeFileName(button->getName());
}

firstPage::~firstPage()
{
    deleteAllChildren();
}

