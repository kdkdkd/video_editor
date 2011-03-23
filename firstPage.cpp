#include "firstPage.h"

class Cloud : public Component
{
    public:
    Image border;
    Cloud(Image& border):Component()
    {
        this->border = border;
    }
    ~Cloud()
    {
        deleteAllChildren();
    }

    void paint(Graphics& g)
    {
        g.setColour(Colours::whitesmoke);
        g.drawImage(border,0,0,getWidth(),getHeight(),0,0,95,31,true);
    }

};


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
        Cloud* current_cloud = recent_list[i];
        current_cloud->setBounds(width/2 - 150, i*35 + delta,300,35);
        current_cloud->getChildComponent(0)->setBounds(0,0,300,35);
    }
    size = localization_buttons.size();
    for(int i = 0;i<size;++i)
    {
        Cloud* current_cloud = localization_buttons[i];
        current_cloud->setBounds(width - 220, i*35 + 20,200,35);
        current_cloud->getChildComponent(0)->setBounds(25,10,175,25);
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
    border = ImageCache::getFromFile(String("../pic/border.png"));
    logo = ImageCache::getFromFile(String("../pic/logo.png"));

    for(int i = 0;i<size;++i)
    {
        if(File(files[i]).exists())
        {
            Cloud * new_cloud = new Cloud(border);
            SpecialButton * new_button = new SpecialButton(files[i]);
            new_button->setButtonText(File(files[i]).getFileName());
            new_cloud->addAndMakeVisible(new_button);
            addAndMakeVisible(new_cloud);
            new_button->addListener(this);
            recent_list.add(new_cloud);
        }
    }


    DirectoryIterator iter(File("../localization/"), false, "*", File::findDirectories);
    while (iter.next())
    {
        Cloud * new_cloud = new Cloud(border);
        File pic_file(File::addTrailingSeparator(iter.getFile().getFullPathName()) + "icon.png");
        DrawableButton* db = new DrawableButton(String::empty, DrawableButton::ImageRaw);
        new_cloud->addAndMakeVisible(db);


        DrawableImage normal,over;
        Image flag = ImageCache::getFromFile(pic_file);
        normal.setImage(flag);

        normal.setOpacity(0.8);
        over.setImage(flag);
        db->setImages(&normal, &over, &normal);

        localization_buttons.add(new_cloud);
        addAndMakeVisible(new_cloud);
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

