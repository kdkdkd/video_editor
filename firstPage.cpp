#include "firstPage.h"
#include "toolbox.h"
#include "events.h"

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
    int delta = getHeight()/2 - size*35;
    if(delta<260)
        delta = 260;
    for(int i = 0;i<size;++i)
    {
        Cloud* current_cloud = recent_list[i];
        current_cloud->setBounds((i%2) * (270) + width - 780, (i/2)*35 + delta,280,35);
        current_cloud->getChildComponent(0)->setBounds(0,0,280,35);
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


void firstPage::loadLocale()
{
    DirectoryIterator iter(File("../localization/"), false, "*", File::findDirectories);
    while (iter.next())
    {
        Cloud * new_cloud = new Cloud(border);
        File pic_file(File::addTrailingSeparator(iter.getFile().getFullPathName()) + "icon.png");

        String iter_locale = iter.getFile().getRelativePathFrom(File("../localization/"));

        DrawableButton* db = new DrawableButton("locale:" + iter_locale, DrawableButton::ImageRaw);
        db->addListener(this);
        db->setMouseCursor(MouseCursor::PointingHandCursor);
        new_cloud->addAndMakeVisible(db);

        DrawableImage normal_image,over_image;

        Image flag = ImageCache::getFromFile(pic_file);
        normal_image.setImage(flag);

        normal_image.setOpacity(0.8);
        over_image.setImage(flag);
        DrawableComposite normal,over;
        RelativeParallelogram newBounds(Rectangle<float>(30,0,115,13));

        normal.setBoundingBox(newBounds);

        DrawableText normal_text,over_text;
        Colour textColour;
        textColour = (iter_locale == localization::current_locale)?Colours::red:Colour(findColour(0x1001f00));

        over_text.setColour(textColour.darker(3.5f));
        normal_text.setColour(textColour);

        normal_text.setBoundingBox(newBounds);
        over_text.setBoundingBox(newBounds);

        File name_file(File::addTrailingSeparator(iter.getFile().getFullPathName()) + "name.txt");
        String lang_name = name_file.loadFileAsString();
        normal_text.setText(lang_name);
        over_text.setText(lang_name);
        normal.insertDrawable(normal_text);
        over.insertDrawable(over_text);



        normal.insertDrawable(normal_image);
        over.insertDrawable(over_image);


        db->setImages(&normal, &over, &normal);
        localization_buttons.add(new_cloud);
        addAndMakeVisible(new_cloud);
    }
}

void UpdateGuiAfterLocalizationChangedInFirstPage(void *object)
{
    firstPage * main = (firstPage *)object;

    String text_video_editor_version = String::empty;
    JUCEApplication *app = JUCEApplication::getInstance();
    text_video_editor_version<<PROGRAM_NAME;
    text_video_editor_version<<":  V";
    text_video_editor_version<<app->getApplicationVersion();
    text_video_editor_version<<",  ";
    text_video_editor_version<<toolbox::get_compilation_date();

    main->label_video_editor_version->setText(text_video_editor_version,false);
    main->label_ffmpeg_version->setText(FFMPEG_STRING,false);
    main->label_juce_version->setText(JUCE_STRING,false);


    if(main->link_ffmpeg_version)
        delete main->link_ffmpeg_version;
    main->link_ffmpeg_version = new HyperlinkButton(VISIT_WEB_SITE,URL(FFMPEG_URL));


    if(main->link_juce_version)
        delete main->link_juce_version;
    main->link_juce_version = new HyperlinkButton(VISIT_WEB_SITE,URL(JUCE_URL));


    if(main->link_video_editor_version)
        delete main->link_video_editor_version;
    main->link_video_editor_version = new HyperlinkButton(VISIT_WEB_SITE,URL(VIDEO_EDITOR_URL));

    Font font = main->label_video_editor_version->getFont();
    font.setHeight(12);

    main->link_video_editor_version->setFont(font,false,Justification::centredRight);
    main->link_video_editor_version->setBounds(250,0,100,40);

    main->cloud_video_editor_version->addAndMakeVisible(main->link_video_editor_version);


    main->link_ffmpeg_version->setFont(font,false,Justification::centredRight);
    main->link_ffmpeg_version->setBounds(250,0,100,40);
    main->cloud_ffmpeg_version->addAndMakeVisible(main->link_ffmpeg_version);

    main->link_juce_version->setFont(font,false,Justification::centredRight);
    main->link_juce_version->setBounds(250,0,100,40);
    main->cloud_juce_version->addAndMakeVisible(main->link_juce_version);
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
            SpecialButton * new_button = new SpecialButton("open:" + files[i]);
            new_button->setButtonText(File(files[i]).getFileName());
            new_cloud->addAndMakeVisible(new_button);
            addAndMakeVisible(new_cloud);
            new_button->addListener(this);
            recent_list.add(new_cloud);
        }
    }

    loadLocale();


    label_video_editor_version = new Label();
    label_ffmpeg_version = new Label();
    label_juce_version = new Label();
    link_video_editor_version = 0;
    link_ffmpeg_version = 0;
    link_juce_version = 0;
    cloud_video_editor_version = new Cloud(border);
    cloud_ffmpeg_version = new Cloud(border);
    cloud_juce_version = new Cloud(border);
    UpdateGuiAfterLocalizationChangedInFirstPage(this);
    AddEvent(AfterLocalizationChnaged,this,UpdateGuiAfterLocalizationChangedInFirstPage);

    Font font = label_video_editor_version->getFont();
    font.setHeight(12);
    label_video_editor_version->setFont(font);
    label_ffmpeg_version->setFont(font);
    label_juce_version->setFont(font);
    cloud_video_editor_version->addAndMakeVisible(label_video_editor_version);
    cloud_ffmpeg_version->addAndMakeVisible(label_ffmpeg_version);
    cloud_juce_version->addAndMakeVisible(label_juce_version);
    addAndMakeVisible(cloud_video_editor_version);
    addAndMakeVisible(cloud_ffmpeg_version);
    addAndMakeVisible(cloud_juce_version);
    cloud_video_editor_version->setBounds(50,70,400,40);
    cloud_ffmpeg_version->setBounds(50,110,400,40);
    cloud_juce_version->setBounds(50,150,400,40);
    label_video_editor_version->setBounds(40,0,200,40);
    label_ffmpeg_version->setBounds(40,0,200,40);
    label_juce_version->setBounds(40,0,200,40);


    Sound *sound = new Sound();
    String file("C:\\1.mov");
    sound->Load(file);
    voice.AddSound(sound);

    /*sound->GotoSecondAndRead(1);
    while(true)
    {
        short res;
        if(!sound->ReadNextByte(&res))
            break;
        printf("%s\n",String(res).toCString());
    }*/



}
void firstPage::buttonClicked(Button* button)
{
    String name = button->getName();
    if(name.startsWith("open:"))
    {
        name = name.substring(5);
        main->changeFileName(name);
    }else if(name.startsWith("locale:"))
    {
        name = name.substring(7);
        load_locale(name);
        int size = localization_buttons.size();
        for(int i = 0;i<size;++i)
        {
            delete localization_buttons[i];
        }
        localization_buttons.clear();
        loadLocale();
        repaint();

        main->mainWindow->setMenuBar(0);
        main->mainWindow->setMenuBar(main);
    }
}

firstPage::~firstPage()
{
    deleteAllChildren();
}

