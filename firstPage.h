#ifndef FIRST_PAGE_H_INCLUDED
#define FIRST_PAGE_H_INCLUDED
#include "config.h"
#include "sound.h"
#include "MainComponent.h"
#include "juce/juce.h"

class MainComponent;
class SpecialButton;
class Cloud;
class firstPage:public Component,public ButtonListener
{
    public:
        void paint(Graphics &g);
        firstPage(MainComponent* main);
        ~firstPage();
        MainComponent* main;
        void buttonClicked(Button* button);
        Array<Cloud*> recent_list;
        Array<Cloud*> localization_buttons;
        void resized();
        Image logo;
        Image border;
        void loadLocale();
        Label * label_video_editor_version;
        Label * label_ffmpeg_version;
        Label * label_juce_version;
        Cloud * cloud_video_editor_version;
        Cloud * cloud_ffmpeg_version;
        Cloud * cloud_juce_version;

        Cloud * open_video;
        Cloud * empty_video;
        Cloud * recent_video_list;
        HyperlinkButton* link_video_editor_version;
        HyperlinkButton* link_ffmpeg_version;
        HyperlinkButton* link_juce_version;

};

#endif
