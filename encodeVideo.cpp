#include "localization.h"
#define upDetailed 10


#include "encodeVideo.h"
encodeVideo::encodeVideo (MainComponent* mainWindow):DocumentWindow(LABEL_SAVE_VIDEO,Colours::whitesmoke,DocumentWindow::closeButton)
{

    setTitleBarHeight (20);

    encodeVideoComponent* contentComponent = new encodeVideoComponent (mainWindow);

    setContentComponent(contentComponent);
    centreAroundComponent(mainWindow, 800, 370 + upDetailed);
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


    String filename = Time::getCurrentTime().formatted(File(mainWindow->timeline->intervals[0]->movie->filename).getFileNameWithoutExtension() + "_%Y%m%d_%H%M%S.avi");
    addAndMakeVisible (path = new FilenameComponent(String::empty,
            File(File::addTrailingSeparator(File::getCurrentWorkingDirectory().getFullPathName()) + filename),
            true,
            false,
            true,
            T("*.*"),
            String::empty,
            String::empty));


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

    setSize (800, 350 + upDetailed);

    /* display all formats and codecs */
    AVOutputFormat *ofmt=NULL;
    int id = 0;
    Movie::Info *movie_info = mainWindow->timeline->intervals.front()->movie->GetMovieInfo();
    while((ofmt = av_oformat_next(ofmt)))
    {
        String id_string = ofmt->name;
        formats.push_back(id_string);
        format->addItem(id_string + "," + String(ofmt->long_name),id++);
    }
    AVCodec *p=NULL;
    int video_index = 0;
    int audio_index = 0;
    while((p = av_codec_next(p)))
    {
        if(p->encode)
        {
            switch(p->type)
            {
            case AVMEDIA_TYPE_VIDEO:
            {
                String id_string = p->name;
                videoCodec->addItem(id_string + "," + String(p->long_name),video_index++);
                video_codecs.push_back(id_string);
            }
            break;
            case AVMEDIA_TYPE_AUDIO:
            {
                String id_string = p->name;
                audioCodec->addItem(id_string + "," + String(p->long_name),audio_index++);
                audio_codecs.push_back(id_string);
            }
            break;
            }
        }
    }
    /* ~display all formats and codecs */
    selectByMovieInfo(movie_info);

    setVisible(true);
}
void encodeVideoComponent::selectByMovieInfo(Movie::Info * info)
{
    /* select format */
    int avi_index = -1;
    String format_short = info->format_short;
    int index_comma = -1;
    if((index_comma = format_short.indexOf(","))>=0)
    {
        format_short = format_short.substring(0,index_comma);
    }
    bool selected_format = false;
    for(int i = 0; i<format->getNumItems(); ++i)
    {
        if(formats[format->getItemId(i)] == "avi")
        {
            avi_index = i;
        }

        if(formats[format->getItemId(i)] == format_short)
        {
            selected_format = true;
            format->setSelectedItemIndex(i);
            break;
        }
    }
    if(!selected_format)
        format->setSelectedItemIndex(avi_index);

    /* ~select format */

    /* select video codec */
    if(info->videos.size()>0)
    {
        bool selected_video_codec = false;
        Movie::VideoInfo video_info = info->videos[0];
        int mpeg4_index = -1;
        for(int i = 0; i<videoCodec->getNumItems(); ++i)
        {
            if(video_codecs[videoCodec->getItemId(i)] == "mpeg4")
            {
                mpeg4_index = i;
            }

            if(video_codecs[videoCodec->getItemId(i)] == video_info.codec_short)
            {
                selected_video_codec = true;
                videoCodec->setSelectedItemIndex(i);
                break;
            }
        }
        if(!selected_video_codec)
            videoCodec->setSelectedItemIndex(mpeg4_index);

        videoWidth->setText(String(video_info.width));
        videoHeight->setText(String(video_info.height));
        if(video_info.bit_rate>0)
            videoBitrate->setText(String(video_info.bit_rate));
        else
            videoBitrate->setText(String("400"));
        fps->setText(String(video_info.fps));
    }
    /* ~select video codec */


    /* select audio codec */
    if(info->audios.size()>0)
    {
        bool selected_audio_codec = false;
        Movie::AudioInfo audio_info = info->audios[0];
        int mp3_index = -1;
        for(int i = 0; i<audioCodec->getNumItems(); ++i)
        {
            if(audio_codecs[audioCodec->getItemId(i)].indexOf("mp3")>=0)
            {
                mp3_index = i;
            }

            if(audio_codecs[audioCodec->getItemId(i)] == audio_info.codec_short)
            {
                selected_audio_codec = true;
                audioCodec->setSelectedItemIndex(i);
                break;
            }
        }
        if(!selected_audio_codec)
            audioCodec->setSelectedItemIndex(mp3_index);


        if(audio_info.sample_rate>0)
            audioSampleRate->setText(String(audio_info.sample_rate));
        else
            audioSampleRate->setText(String("441"));
        if(audio_info.bit_rate>0)
            audioBitrate->setText(String(audio_info.bit_rate));
        else
            audioBitrate->setText(String("64"));
        channels->setText(String(audio_info.channels));
    }
    /* ~select audio codec */
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
    g.drawFittedText (LABEL_VIDEO_SAVE_PATH,
                      12, 4 + upDetailed, 200, 30,2,
                      Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawFittedText (LABEL_VIDEO_SAVE_FORMAT,
                      12, 44+ upDetailed, 200, 30,2,
                      Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawFittedText (LABEL_VIDEO_SAVE_BITRATE,
                      0, 204+ upDetailed, 148-20, 30,2,
                      Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawFittedText (LABEL_VIDEO_SAVE_RESOLUTION,
                      0, 164+ upDetailed, 148-20, 30,2,
                      Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawFittedText (LABEL_VIDEO_SAVE_CODEC,
                      0, 124+ upDetailed, 148-20, 30,2,
                      Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawFittedText (LABEL_VIDEO_SAVE_FPS,
                      0, 244+ upDetailed, 148-20, 30,2,
                      Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawText (T("X"),
                300-28, 164+ upDetailed, 32, 30,
                Justification::centred, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawFittedText (LABEL_AUDIO_SAVE_CODEC,
                      420-20, 124+ upDetailed, 108, 30,2,
                      Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawFittedText (LABEL_AUDIO_SAVE_BITRATE,
                      420-20, 164+ upDetailed, 108, 30,2,
                      Justification::centredRight, true);

    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawFittedText (LABEL_AUDIO_SAVE_SAMPLE_RATE,
                      420-20, 204+ upDetailed, 108, 30,2,
                      Justification::centredRight, true);


    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawFittedText (LABEL_AUDIO_SAVE_CHANNELS,
                      420-20, 244+ upDetailed, 108, 30,2,
                      Justification::centredRight, true);

    g.drawFittedText (LABEL_KB_PER_SECOND,
                      200-48 + 187, 208+ upDetailed, 45, 24,2,
                      Justification::centred, true);

    g.drawFittedText (LABEL_KB_PER_SECOND,
                      535-20 + 202, 168+ upDetailed, 45, 24,2,
                      Justification::centred, true);

    g.drawFittedText ("Hz",
                      535-20 + 202, 208+ upDetailed, 45, 24,2,
                      Justification::centred, true);




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
    format->setBounds (232, 48+ upDetailed, 540, 24);
    path->setBounds (232, 8+ upDetailed, 540, 24);
    groupComponent->setBounds (16, 104+ upDetailed, 380, 184);
    videoCodec->setBounds (200-48, 128+ upDetailed, 232, 24);
    videoWidth->setBounds (200-48, 168+ upDetailed, 88, 24);
    videoHeight->setBounds (344-48, 168+ upDetailed, 88, 24);
    videoBitrate->setBounds (200-48, 208+ upDetailed, 182, 24);
    fps->setBounds (200-48, 248+ upDetailed, 232, 24);
    groupComponent2->setBounds (400, 104+ upDetailed, 380, 184);
    audioCodec->setBounds (535-20, 128+ upDetailed, 252, 24);
    audioBitrate->setBounds (535-20, 168+ upDetailed, 202, 24);
    audioSampleRate->setBounds (535-20, 208+ upDetailed, 202, 24);
    channels->setBounds (535-20, 248+ upDetailed, 252, 24);
    ok->setBounds (72+380, 300 + upDetailed, 150, 24);
    cancel->setBounds (248+380, 300 + upDetailed, 150, 24);
}

void encodeVideoComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == format)
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

