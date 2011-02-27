#ifndef __JUCER_HEADER_ENCODEVIDEO_ENCODEVIDEO_19114A34__
#define __JUCER_HEADER_ENCODEVIDEO_ENCODEVIDEO_19114A34__

#include "videoPreview.h"
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
    void add ();
};
class videoPreview;
class encodeVideoComponent  : public ComboBoxListener,
                     public ButtonListener,
                     public Component,
                     public TextEditor::Listener,
                     public FilenameComponentListener
{
public:
    TooltipWindow tooltipWindow;
    encodeVideoComponent(MainComponent* mainWindow);
    ~encodeVideoComponent();

    void textEditorReturnKeyPressed (TextEditor& editor);
    void textEditorEscapeKeyPressed (TextEditor& editor);
    void textEditorFocusLost (TextEditor& editor);

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void buttonClicked (Button* buttonThatWasClicked);
    void selectByMovieInfo(Movie::Info * info);
    void clearValidation();
    bool Validate();
    void textEditorTextChanged(TextEditor& editor);
    ComboBox* qualityList;
    ComboBox* resolutionList;
    File getCurrentFileName();
    FilenameComponent* path;

    ComboBox* format;
    bool gopSetByUser;
    ComboBox* rateControl;
    TextEditor* crf;
    void recalculateCRF();
    void recalculateBitrate();
    void UpdateFileExtension(bool forse_extension);
    void filenameComponentChanged (FilenameComponent* fileComponentThatHasChanged);
    videoPreview * preview;
    ToggleButton* showPreview;
    Timeline* timeline;
    bool isPreviewVisible;
    MainComponent* mainWindow;
    Movie::Info GetMovieInfo();
private:

    GroupComponent* groupComponent;
    ComboBox* videoCodec;

    ComboBox* passList;

    ToggleButton* advancedMode;

    bool isAdvancedMode;
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
    TextEditor* gop;

    encodeVideoComponent (const encodeVideo&);
    const encodeVideoComponent& operator= (const encodeVideo&);

    ComboBox* compressionPreset;
    bool hasCompressionPreset;
    ToggleButton* enableVideo;
    bool isVideoEnabled;
    ToggleButton* enableAudio;
    bool isAudioEnabled;
    int previous_format;
    int previous_codec;
};


#endif   // __JUCER_HEADER_ENCODEVIDEO_ENCODEVIDEO_19114A34__
