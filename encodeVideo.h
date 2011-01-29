#ifndef __JUCER_HEADER_ENCODEVIDEO_ENCODEVIDEO_19114A34__
#define __JUCER_HEADER_ENCODEVIDEO_ENCODEVIDEO_19114A34__


#include "MainComponent.h"
#include "juce/juce.h"
#include <vector>
using namespace std;
class MainComponent;

class encodeVideo  : DocumentWindow
{
    public:
    encodeVideo(MainComponent* mainWindow);
    ~encodeVideo();
    void closeButtonPressed();
};

class encodeVideoComponent  : public ComboBoxListener,
                     public ButtonListener,
                     public Component
{
public:

    encodeVideoComponent(MainComponent* mainWindow);
    ~encodeVideoComponent();


    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void buttonClicked (Button* buttonThatWasClicked);
    void selectByMovieInfo(Movie::Info * info);



private:

        MainComponent* mainWindow;


    ComboBox* format;
    FilenameComponent* path;

    GroupComponent* groupComponent;
    ComboBox* videoCodec;
    TextEditor* videoWidth;
    TextEditor* videoHeight;
    TextEditor* videoBitrate;
    TextEditor* fps;
    GroupComponent* groupComponent2;
    ComboBox* audioCodec;
    TextEditor* audioBitrate;
    TextEditor* audioSampleRate;
    TextEditor* channels;
    TextButton* ok;
    TextButton* cancel;
    vector<String> formats;
    vector<String> audio_codecs;
    vector<String> video_codecs;


    encodeVideoComponent (const encodeVideo&);
    const encodeVideoComponent& operator= (const encodeVideo&);
};


#endif   // __JUCER_HEADER_ENCODEVIDEO_ENCODEVIDEO_19114A34__
