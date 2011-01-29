#include "localization.h"
#define upDetailed 10


#include "encodeVideo.h"
encodeVideo::encodeVideo (MainComponent* mainWindow):DocumentWindow(LABEL_SAVE_VIDEO,Colours::whitesmoke,DocumentWindow::closeButton)
{

    setTitleBarHeight (20);

    encodeVideoComponent* contentComponent = new encodeVideoComponent (mainWindow);

    setContentComponent(contentComponent);
    centreAroundComponent(mainWindow, 470, 570 + upDetailed);
    setVisible(true);
    addToDesktop(ComponentPeer::windowHasCloseButton || ComponentPeer::windowHasTitleBar);
}
void encodeVideo::closeButtonPressed()
{
    removeFromDesktop();
    delete this->getContentComponent();
}
encodeVideo::~encodeVideo()
{

}

encodeVideoComponent::encodeVideoComponent (MainComponent* mainWindow)
    : format (0),
      path (0),
      groupComponent (0),
      videoCodec (0),
      videoWidth (0),
      videoHeight (0),
      videoBitrate (0),
      fps (0),
      groupComponent2 (0),
      audioCodec (0),
      audioBitrate (0),
      audioSampleRate (0),
      channels (0),
      ok (0),
      cancel (0),
      Component("encodeVideoComponent")

{
    addAndMakeVisible (format = new ComboBox ());
    format->setEditableText (false);
    format->setJustificationType (Justification::centredLeft);
    format->setTextWhenNothingSelected (String::empty);
    format->addListener (this);

    addAndMakeVisible (path = new ComboBox ());
    path->setEditableText (false);
    path->setJustificationType (Justification::centredLeft);
    path->setTextWhenNothingSelected (String::empty);
    path->addListener (this);

    addAndMakeVisible (groupComponent = new GroupComponent (T("g1"),
                                                            LABEL_VIDEO));

    addAndMakeVisible (videoCodec = new ComboBox ());
    videoCodec->setEditableText (false);
    videoCodec->setJustificationType (Justification::centredLeft);
    videoCodec->setTextWhenNothingSelected (String::empty);
    videoCodec->addListener (this);

    addAndMakeVisible (videoWidth = new TextEditor ());
    videoWidth->setMultiLine (false);
    videoWidth->setReturnKeyStartsNewLine (false);
    videoWidth->setReadOnly (false);
    videoWidth->setScrollbarsShown (true);
    videoWidth->setCaretVisible (true);
    videoWidth->setPopupMenuEnabled (true);
    videoWidth->setText (String::empty);

    addAndMakeVisible (videoHeight = new TextEditor ());
    videoHeight->setMultiLine (false);
    videoHeight->setReturnKeyStartsNewLine (false);
    videoHeight->setReadOnly (false);
    videoHeight->setScrollbarsShown (true);
    videoHeight->setCaretVisible (true);
    videoHeight->setPopupMenuEnabled (true);
    videoHeight->setText (String::empty);

    addAndMakeVisible (videoBitrate = new TextEditor ());
    videoBitrate->setMultiLine (false);
    videoBitrate->setReturnKeyStartsNewLine (false);
    videoBitrate->setReadOnly (false);
    videoBitrate->setScrollbarsShown (true);
    videoBitrate->setCaretVisible (true);
    videoBitrate->setPopupMenuEnabled (true);
    videoBitrate->setText (String::empty);

    addAndMakeVisible (fps = new TextEditor ());
    fps->setMultiLine (false);
    fps->setReturnKeyStartsNewLine (false);
    fps->setReadOnly (false);
    fps->setScrollbarsShown (true);
    fps->setCaretVisible (true);
    fps->setPopupMenuEnabled (true);
    fps->setText (String::empty);

    addAndMakeVisible (groupComponent2 = new GroupComponent (T("g2"),
                                                             LABEL_AUDIO));

    addAndMakeVisible (audioCodec = new ComboBox ());
    audioCodec->setEditableText (false);
    audioCodec->setJustificationType (Justification::centredLeft);
    audioCodec->setTextWhenNothingSelected (String::empty);
    audioCodec->addListener (this);

    addAndMakeVisible (audioBitrate = new TextEditor ());
    audioBitrate->setMultiLine (false);
    audioBitrate->setReturnKeyStartsNewLine (false);
    audioBitrate->setReadOnly (false);
    audioBitrate->setScrollbarsShown (true);
    audioBitrate->setCaretVisible (true);
    audioBitrate->setPopupMenuEnabled (true);
    audioBitrate->setText (String::empty);

    addAndMakeVisible (audioSampleRate = new TextEditor ());
    audioSampleRate->setMultiLine (false);
    audioSampleRate->setReturnKeyStartsNewLine (false);
    audioSampleRate->setReadOnly (false);
    audioSampleRate->setScrollbarsShown (true);
    audioSampleRate->setCaretVisible (true);
    audioSampleRate->setPopupMenuEnabled (true);
    audioSampleRate->setText (String::empty);

    addAndMakeVisible (channels = new TextEditor ());
    channels->setMultiLine (false);
    channels->setReturnKeyStartsNewLine (false);
    channels->setReadOnly (false);
    channels->setScrollbarsShown (true);
    channels->setCaretVisible (true);
    channels->setPopupMenuEnabled (true);
    channels->setText (String::empty);

    addAndMakeVisible (ok = new TextButton ());
    ok->setButtonText (LABEL_SAVE);
    ok->addListener (this);

    addAndMakeVisible (cancel = new TextButton ());
    cancel->setButtonText (LABEL_CANCEL);
    cancel->addListener (this);


    this->mainWindow = mainWindow;


    setSize (470, 550 + upDetailed);

    setVisible(true);
}

encodeVideoComponent::~encodeVideoComponent()
{
    deleteAndZero (format);
    deleteAndZero (path);
    deleteAndZero (groupComponent);
    deleteAndZero (videoCodec);
    deleteAndZero (videoWidth);
    deleteAndZero (videoHeight);
    deleteAndZero (videoBitrate);
    deleteAndZero (fps);
    deleteAndZero (groupComponent2);
    deleteAndZero (audioCodec);
    deleteAndZero (audioBitrate);
    deleteAndZero (audioSampleRate);
    deleteAndZero (channels);
    deleteAndZero (ok);
    deleteAndZero (cancel);

}

//==============================================================================
void encodeVideoComponent::paint (Graphics& g)
{
    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawText (LABEL_VIDEO_SAVE_PATH,
                12, 4 + upDetailed, 200, 30,
                Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawText (LABEL_VIDEO_SAVE_FORMAT,
                12, 44+ upDetailed, 200, 30,
                Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawText (LABEL_VIDEO_SAVE_CODEC,
                28, 204+ upDetailed, 148, 30,
                Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawText (LABEL_VIDEO_SAVE_RESOLUTION,
                28, 164+ upDetailed, 148, 30,
                Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawText (LABEL_VIDEO_SAVE_BITRATE,
                28, 124+ upDetailed, 148, 30,
                Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawText (LABEL_VIDEO_SAVE_FPS,
                28, 244+ upDetailed, 148, 30,
                Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawText (T("X"),
                300, 164+ upDetailed, 32, 30,
                Justification::centred, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawText (LABEL_AUDIO_SAVE_CODEC,
                20, 324+ upDetailed, 148, 30,
                Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawText (LABEL_AUDIO_SAVE_BITRATE,
                20, 364+ upDetailed, 148, 30,
                Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (14.3000f, Font::plain));
    g.drawText (LABEL_AUDIO_SAVE_SAMPLE_RATE,
                20, 404+ upDetailed, 148, 30,
                Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawText (LABEL_AUDIO_SAVE_CHANNELS,
                20, 444+ upDetailed, 148, 30,
                Justification::centredRight, true);


    Image backgroundFill = Image (Image::ARGB, 10, 10, true);
    {
        Graphics g (backgroundFill);
        Colour col (Colours::black);
        g.setColour (col.withAlpha (0.1f));
        g.drawRect (0, 0, 10 + 1, 10 + 1);
        g.setColour (col.withAlpha (0.35f));
        g.setPixel (0, 0);
    }

    if (backgroundFill.isValid())
    {
        g.setTiledImageFill (backgroundFill, 0, 0, 1.0f);
        g.fillAll();
    }
    g.drawImageWithin(*(mainWindow->timeline->intervals[0]->preview),20,10,128,96 ,RectanglePlacement::centred,false);


}

void encodeVideoComponent::resized()
{
    format->setBounds (232, 48+ upDetailed, 150, 24);
    path->setBounds (232, 8+ upDetailed, 150, 24);
    groupComponent->setBounds (16, 104+ upDetailed, 432, 184);
    videoCodec->setBounds (200, 128+ upDetailed, 232, 24);
    videoWidth->setBounds (200, 168+ upDetailed, 88, 24);
    videoHeight->setBounds (344, 168+ upDetailed, 88, 24);
    videoBitrate->setBounds (200, 208+ upDetailed, 232, 24);
    fps->setBounds (200, 248+ upDetailed, 232, 24);
    groupComponent2->setBounds (16, 304+ upDetailed, 432, 184);
    audioCodec->setBounds (200, 328+ upDetailed, 232, 24);
    audioBitrate->setBounds (200, 368+ upDetailed, 232, 24);
    audioSampleRate->setBounds (200, 408+ upDetailed, 232, 24);
    channels->setBounds (200, 448+ upDetailed, 232, 24);
    ok->setBounds (72, 504 + upDetailed, 150, 24);
    cancel->setBounds (248, 504 + upDetailed, 150, 24);
}

void encodeVideoComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == format)
    {

    }
    else if (comboBoxThatHasChanged == path)
    {

    }
    else if (comboBoxThatHasChanged == videoCodec)
    {

    }
    else if (comboBoxThatHasChanged == audioCodec)
    {

    }

}

void encodeVideoComponent::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == ok)
    {
           getParentComponent()->removeFromDesktop();
    }
    else if (buttonThatWasClicked == cancel)
    {
           getParentComponent()->removeFromDesktop();
     }


}

