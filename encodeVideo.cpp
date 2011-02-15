#include "localization.h"
#define upDetailed 50
#include "capabilities.h"

#include "encodeVideo.h"
encodeVideo::encodeVideo (MainComponent* mainWindow):DocumentWindow(LABEL_SAVE_VIDEO,Colours::whitesmoke,DocumentWindow::closeButton)
{

    setTitleBarHeight (20);

    encodeVideoComponent* contentComponent = new encodeVideoComponent (mainWindow);

    setContentComponent(contentComponent);
    centreAroundComponent(mainWindow, 800, 370 + upDetailed+200);
    setVisible(true);
    addToDesktop(ComponentPeer::windowHasCloseButton || ComponentPeer::windowHasTitleBar);
}

void encodeVideo::add()
{
    encodeVideoComponent *child = (encodeVideoComponent*)getContentComponent();
    bool need_recalculate_crf = false;
    if(child->qualityList->getSelectedId() == 100)
    {
        child->comboBoxChanged(child->qualityList);
        need_recalculate_crf = true;
    }
    if(child->resolutionList->getSelectedId() == 1)
    {
        child->comboBoxChanged(child->resolutionList);
        need_recalculate_crf = true;
    }
    if(need_recalculate_crf)
        child->recalculateCRF();
    setVisible(true);
    addToDesktop(ComponentPeer::windowHasCloseButton || ComponentPeer::windowHasTitleBar);
}
void encodeVideoComponent::UpdateFileExtension(bool forse_extension)
{
    File temp(path->getCurrentFile());
    capabilities::Format selected_format = capabilities::formats.at(format->getSelectedId()-1);
    String full_path = temp.getFullPathName();
    int last_index_of_dot = full_path.lastIndexOf(".");

    String new_file_with_ext;
    if(last_index_of_dot>=0)
    {
        if(!forse_extension)
            return;
        new_file_with_ext = full_path.substring(0,last_index_of_dot);
    }
    else
    {
        new_file_with_ext = full_path;
    }
    new_file_with_ext<<"."<<capabilities::formats.at(format->getSelectedId()-1).display_id;
    path->setCurrentFile(File(new_file_with_ext),false,false);
}
void encodeVideoComponent::recalculateCRF()
{
    if(rateControl->getSelectedId()>1)
    {
        int quality_id = qualityList->getSelectedId();
        if(quality_id>-20)
        {
            double rate;
            switch(quality_id)
            {
            case -3:
                rate = 0.8;
                break;
            case -2:
                rate = 0.6;
                break;
            case -1:
                rate = 0.4;
                break;
            case 100:
            case 1:
                rate = 0.3;
                break;
            case 2:
                rate = 0.2;
                break;
            case 3:
                rate = 0.1;
                break;
            case 4:
                rate = 0.0;
                break;
            }
            capabilities::VideoCodec vc = capabilities::video_codecs[videoCodec->getSelectedId()-1];
            crf->setText(String(int(double(vc.qmax - vc.qmin) * rate + vc.qmin)),false);

        }
    }
}
void encodeVideoComponent::recalculateBitrate()
{
    int id = qualityList->getSelectedId();
    int normal_bitrate = videoWidth->getText().getIntValue() * videoHeight->getText().getIntValue() / 150 ;

    switch(id)
    {
    case 4:
        normal_bitrate*=3;
        break;
    case 3:
        normal_bitrate*=2;
        break;
    case 2:
        normal_bitrate*=3;
        normal_bitrate/=2;
        break;
    case -1:
        normal_bitrate/=2;
        break;
    case -2:
        normal_bitrate/=4;
        break;
    case -3:
        normal_bitrate/=8;
        break;
    case 100:
    {
        int bit_rate_max = 0;
        for(vector<Timeline::Interval*>::iterator it = mainWindow->timeline->intervals.begin(); it!=mainWindow->timeline->intervals.end(); ++it)
        {
            int bit_rate = (*it)->movie->GetMovieInfo()->bit_rate;
            if((*it)->movie->GetMovieInfo()->videos.size()>0)
            {
                int bit_rate_candidate = (*it)->movie->GetMovieInfo()->videos[0].bit_rate;
                if(bit_rate_candidate>0)
                    bit_rate = bit_rate_candidate;
            }
            if(bit_rate>bit_rate_max)
            {
                bit_rate_max = bit_rate;
            }
        }
        if(bit_rate_max>0)
            normal_bitrate = bit_rate_max;
    }
    break;
    case -100:
    {
        if(rateControl->getSelectedId()==1)
        {
            if(!isAdvancedMode)
                advancedMode->setToggleState(true,true);
            videoBitrate->setSelectAllWhenFocused(true);
            videoBitrate->grabKeyboardFocus();
            videoBitrate->setSelectAllWhenFocused(false);
            return;
        }
        else
        {
            if(!isAdvancedMode)
                advancedMode->setToggleState(true,true);
            crf->setSelectAllWhenFocused(true);
            crf->grabKeyboardFocus();
            crf->setSelectAllWhenFocused(false);
            return;
        }
    }
    break;
    }

    videoBitrate->setText(String(normal_bitrate),false);
}
void encodeVideo::closeButtonPressed()
{
    removeFromDesktop();
}
encodeVideo::~encodeVideo()
{

}

void encodeVideoComponent::textEditorReturnKeyPressed (TextEditor& editor)
{
}
void encodeVideoComponent::textEditorEscapeKeyPressed (TextEditor& editor)
{
}
void encodeVideoComponent::textEditorFocusLost (TextEditor& editor)
{

}

void encodeVideoComponent::textEditorTextChanged(TextEditor& editor)
{

    String name = editor.getName();
    if(name == "videoWidth" || name == "videoHeight")
    {
        resolutionList->setSelectedId(-100,false);
    }
    else if(name == "videoBitrate" || name == "crf")
    {
        qualityList->setSelectedId(-100,false);
    }
    else if(name = "gop")
    {
        gopSetByUser = true;
    }

    clearValidation();
    Validate();
}
File encodeVideoComponent::getCurrentFileName()
{
    String filename = Time::getCurrentTime().formatted(File(mainWindow->timeline->intervals[0]->movie->filename).getFileNameWithoutExtension() + "_%Y%m%d_%H%M%S.avi");
    return File(File::addTrailingSeparator(File::getCurrentWorkingDirectory().getFullPathName()) + filename);
}
encodeVideoComponent::encodeVideoComponent (MainComponent* mainWindow)
    : format (0),
    path (0),
    groupComponent (0),
    videoCodec (0),
    videoWidth (0),
    videoHeight (0),
    fps (0),
    groupComponent2 (0),
    audioCodec (0),
    audioBitrate (0),
    audioSampleRate (0),
    channels (0),
    ok (0),
    cancel (0),
    Component("encodeVideoComponent"),
    hasCompressionPreset(false)
{
    gopSetByUser = false;
    this->mainWindow = mainWindow;
    addAndMakeVisible (compressionPreset = new ComboBox ());
    compressionPreset->setEditableText (false);
    compressionPreset->setJustificationType (Justification::centredLeft);
    compressionPreset->setTextWhenNothingSelected (String::empty);
    compressionPreset->addListener (this);

    addAndMakeVisible (format = new ComboBox ());
    format->setEditableText (false);
    format->setJustificationType (Justification::centredLeft);
    format->setTextWhenNothingSelected (String::empty);
    format->addListener (this);


    addAndMakeVisible (path = new FilenameComponent(String::empty,
            getCurrentFileName(),
            true,
            false,
            true,
            T("*.*"),
            String::empty,
            String::empty));

    path->addListener(this);

    addAndMakeVisible (groupComponent = new GroupComponent (T("g1"),
            LABEL_VIDEO));

    addAndMakeVisible (videoCodec = new ComboBox ());
    videoCodec->setEditableText (false);
    videoCodec->setJustificationType (Justification::centredLeft);
    videoCodec->setTextWhenNothingSelected (String::empty);
    videoCodec->addListener (this);

    addAndMakeVisible (rateControl = new ComboBox ());
    rateControl->setEditableText (false);
    rateControl->setJustificationType (Justification::centredLeft);
    rateControl->setTextWhenNothingSelected (String::empty);
    rateControl->addListener (this);


    addAndMakeVisible (resolutionList = new ComboBox ());
    resolutionList->setEditableText (false);
    resolutionList->setJustificationType (Justification::centredLeft);
    resolutionList->setTextWhenNothingSelected (String::empty);
    resolutionList->addListener (this);


    addAndMakeVisible (passList = new ComboBox ());
    passList->setEditableText (false);
    passList->setJustificationType (Justification::centredLeft);
    passList->setTextWhenNothingSelected (String::empty);
    passList->addListener (this);


    addAndMakeVisible (qualityList = new ComboBox ());
    qualityList->setEditableText (false);
    qualityList->setJustificationType (Justification::centredLeft);
    qualityList->setTextWhenNothingSelected (String::empty);
    qualityList->addListener (this);

    addAndMakeVisible (advancedMode= new ToggleButton (LABEL_VIDEO_SAVE_ADVANCED_MODE));
    advancedMode->setToggleState (true, false);
    advancedMode->addListener (this);
    isAdvancedMode = true;

    addAndMakeVisible (enableVideo= new ToggleButton (LABEL_VIDEO_SAVE_ENABLE_VIDEO));
    enableVideo->setToggleState (true, false);
    enableVideo->addListener (this);
    isVideoEnabled = true;

    addAndMakeVisible (enableAudio= new ToggleButton (LABEL_VIDEO_SAVE_ENABLE_AUDIO));
    enableAudio->setToggleState (true, false);
    enableAudio->addListener (this);
    isAudioEnabled = true;


    String digits = T("0123456789");
    String digits_and_dot = T("0123456789.");

    addAndMakeVisible (videoWidth = new TextEditor (T("videoWidth")));
    videoWidth->setInputRestrictions(4,digits);
    videoWidth->setMultiLine (false);
    videoWidth->setReturnKeyStartsNewLine (false);
    videoWidth->setReadOnly (false);
    videoWidth->setScrollbarsShown (true);
    videoWidth->setCaretVisible (true);
    videoWidth->setPopupMenuEnabled (true);
    videoWidth->setText (String::empty);
    videoWidth->addListener (this);

    addChildComponent(crf = new TextEditor (T("crf")));
    crf->setInputRestrictions(2,digits);
    crf->setMultiLine (false);
    crf->setReturnKeyStartsNewLine (false);
    crf->setReadOnly (false);
    crf->setScrollbarsShown (true);
    crf->setCaretVisible (true);
    crf->setPopupMenuEnabled (true);
    crf->setText (String::empty);
    crf->addListener (this);

    addAndMakeVisible (gop = new TextEditor ("gop"));
    gop->setInputRestrictions(3,digits);
    gop->setMultiLine (false);
    gop->setReturnKeyStartsNewLine (false);
    gop->setReadOnly (false);
    gop->setScrollbarsShown (true);
    gop->setCaretVisible (true);
    gop->setPopupMenuEnabled (true);
    gop->setText (String::empty,false);
    gop->addListener(this);

    addAndMakeVisible (videoHeight = new TextEditor (T("videoHeight")));
    videoHeight->setInputRestrictions(4,digits);
    videoHeight->setMultiLine (false);
    videoHeight->setReturnKeyStartsNewLine (false);
    videoHeight->setReadOnly (false);
    videoHeight->setScrollbarsShown (true);
    videoHeight->setCaretVisible (true);
    videoHeight->setPopupMenuEnabled (true);
    videoHeight->setText (String::empty);
    videoHeight->addListener (this);

    addAndMakeVisible (videoBitrate = new TextEditor (T("videoBitrate")));
    videoBitrate->setInputRestrictions(5,digits);
    videoBitrate->setMultiLine (false);
    videoBitrate->setReturnKeyStartsNewLine (false);
    videoBitrate->setReadOnly (false);
    videoBitrate->setScrollbarsShown (true);
    videoBitrate->setCaretVisible (true);
    videoBitrate->setPopupMenuEnabled (true);
    videoBitrate->setText (String::empty);
    videoBitrate->addListener (this);

    addAndMakeVisible (fps = new TextEditor ());
    fps->setInputRestrictions(0,digits_and_dot);
    fps->setMultiLine (false);
    fps->setReturnKeyStartsNewLine (false);
    fps->setReadOnly (false);
    fps->setScrollbarsShown (true);
    fps->setCaretVisible (true);
    fps->setPopupMenuEnabled (true);
    fps->setText (String::empty);
    fps->addListener(this);

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


    setSize (800, 350 + upDetailed+120+40);

    /* display all formats and codecs */

    int id = 0;

    for(vector<capabilities::Format>::iterator it=capabilities::formats.begin(); it!=capabilities::formats.end(); ++it)
    {
        if(it->header!=String::empty)format->addSectionHeading(it->header);
        format->addItem(it->display_id + "," + String(it->description),++id);

    }
    /*id = 0;
    for(vector<capabilities::VideoCodec>::iterator it=capabilities::video_codecs.begin();it!=capabilities::video_codecs.end();++it)
    {
        videoCodec->addItem(it->display_id + "," + String(it->description),++id);
    }*/
    id = 0;
    for(vector<capabilities::FFMpegUnit>::iterator it=capabilities::audio_codecs.begin(); it!=capabilities::audio_codecs.end(); ++it)
    {
        audioCodec->addItem(it->display_id + "," + String(it->description),++id);
    }


    qualityList->addItem(LABEL_VIDEO_SAVE_AUTO,100);
    qualityList->addItem(LABEL_VIDEO_SAVE_QUALITY_BEST,4);
    qualityList->addItem(LABEL_VIDEO_SAVE_QUALITY_BETTER,3);
    qualityList->addItem(LABEL_VIDEO_SAVE_QUALITY_GOOD,2);
    qualityList->addItem(LABEL_VIDEO_SAVE_QUALITY_NORMAL,1);
    qualityList->addItem(LABEL_VIDEO_SAVE_QUALITY_BAD,-1);
    qualityList->addItem(LABEL_VIDEO_SAVE_QUALITY_WORSE,-2);
    qualityList->addItem(LABEL_VIDEO_SAVE_QUALITY_WORST,-3);
    qualityList->addItem(LABEL_VIDEO_SAVE_QUALITY_OTHER,-100);

    qualityList->setSelectedId(100);

    compressionPreset->addItem(LABEL_VIDEO_SAVE_COMPRESSION_ULTRAFAST,10);
    compressionPreset->addItem(LABEL_VIDEO_SAVE_COMPRESSION_SUPERFAST,9);
    compressionPreset->addItem(LABEL_VIDEO_SAVE_COMPRESSION_VERYFAST,8);
    compressionPreset->addItem(LABEL_VIDEO_SAVE_COMPRESSION_FASTER,7);
    compressionPreset->addItem(LABEL_VIDEO_SAVE_COMPRESSION_FAST,6);
    compressionPreset->addItem(LABEL_VIDEO_SAVE_COMPRESSION_MEDIUM,5);
    compressionPreset->addItem(LABEL_VIDEO_SAVE_COMPRESSION_SLOW,4);
    compressionPreset->addItem(LABEL_VIDEO_SAVE_COMPRESSION_SLOWER,3);
    compressionPreset->addItem(LABEL_VIDEO_SAVE_COMPRESSION_VERYSLOW,2);
    compressionPreset->addItem(LABEL_VIDEO_SAVE_COMPRESSION_PLACEBO,1);
    compressionPreset->setSelectedId(7);

    passList->addItem(LABEL_VIDEO_SAVE_PASS_ONE,1);
    passList->addItem(LABEL_VIDEO_SAVE_PASS_TWO,2);

    /* ~display all formats and codecs */
    Movie::Info *movie_info = mainWindow->timeline->intervals.front()->movie->GetMovieInfo();
    selectByMovieInfo(movie_info);


    setVisible(true);
}
void encodeVideoComponent::selectByMovieInfo(Movie::Info * info)
{
    /* select video codec */
    int index = -1;
    for(int i = 0; i<format->getNumItems(); ++i)
    {
        if(capabilities::formats[format->getItemId(i)-1].display_id == "mkv")
        {
            index = i;
            break;
        }
    }

    format->setSelectedItemIndex(index);

    Movie::VideoInfo video_info = info->videos[0];
    gop->setText(String(""),false);
    fps->setText(String(video_info.fps),false);
    passList->setSelectedItemIndex(0);

    /* ~select video codec */
    /* select audio codec */


    for(int i = 0; i<audioCodec->getNumItems(); ++i)
    {
        if(capabilities::audio_codecs[audioCodec->getItemId(i)-1].display_id == "ac3")
        {
            index = i;
            break;
        }
    }
    audioCodec->setSelectedItemIndex(index);
    audioSampleRate->setText(String("441"));
    audioBitrate->setText(String("64"));
    channels->setText(String("2"));

    /* ~select audio codec */
}


Movie::Info encodeVideoComponent::GetMovieInfo()
{
    Movie::Info res;
    if(format->getSelectedId())
        res.format_short = capabilities::formats[format->getSelectedId()-1].id;
    res.filename = path->getCurrentFile().getFullPathName();
    if(isVideoEnabled)
    {
        Movie::VideoInfo video_info;
        capabilities::VideoCodec vc = capabilities::video_codecs[videoCodec->getSelectedId()-1];
        video_info.codec_short = vc.id;
        video_info.width = videoWidth->getText().getIntValue();
        video_info.height = videoHeight->getText().getIntValue();
        video_info.fps = fps->getText().getDoubleValue();
        video_info.compressionPreset = -1;
        video_info.pass = passList->getSelectedId();
        if(vc.hasCompressionPreset())
            video_info.compressionPreset = compressionPreset->getSelectedId();

        video_info.is_bitrate_or_crf = rateControl->getSelectedId()==1;
        video_info.gop = gop->getText().getIntValue();
        video_info.bit_rate = (video_info.is_bitrate_or_crf)?videoBitrate->getText().getIntValue():crf->getText().getIntValue();
        res.videos.push_back(video_info);

    }
    if(isAudioEnabled)
    {
        Movie::AudioInfo audio_info;
        audio_info.codec_short = capabilities::audio_codecs[audioCodec->getSelectedId()-1].id;
        audio_info.bit_rate = audioBitrate->getText().getIntValue();
        audio_info.sample_rate = audioSampleRate->getText().getIntValue();
        audio_info.channels = channels->getText().getIntValue();
        res.audios.push_back(audio_info);
    }
    return res;
}


encodeVideoComponent::~encodeVideoComponent()
{
    deleteAndZero (format);
    deleteAndZero (advancedMode);
    deleteAndZero (resolutionList);
    deleteAndZero (passList);
    deleteAndZero (qualityList);
    deleteAndZero (path);
    deleteAndZero (groupComponent);
    deleteAndZero (videoCodec);
    deleteAndZero (videoWidth);
    deleteAndZero (videoHeight);
    deleteAndZero (videoBitrate);
    deleteAndZero (fps);
    deleteAndZero (gop);
    deleteAndZero (groupComponent2);
    deleteAndZero (audioCodec);
    deleteAndZero (audioBitrate);
    deleteAndZero (audioSampleRate);
    deleteAndZero (channels);
    deleteAndZero (ok);
    deleteAndZero (cancel);
    deleteAndZero (compressionPreset);
    deleteAndZero (enableAudio);
    deleteAndZero (enableVideo);
    deleteAndZero (crf);
}

//==============================================================================
void encodeVideoComponent::paint (Graphics& g)
{
    g.setColour (Colours::black);
    g.setFont (Font (15.0000f, Font::plain));
    g.drawFittedText (LABEL_VIDEO_SAVE_PATH,
                      12, 4, 200, 30,2,
                      Justification::centredRight, true);


    g.drawFittedText (LABEL_VIDEO_SAVE_FORMAT,
                      12, 44, 200, 30,2,
                      Justification::centredRight, true);

    int add = 0;
    if(hasCompressionPreset)
        add = 40;

    /* video labels */
    if(!isVideoEnabled)
        g.setColour (Colours::grey);
    if(isAdvancedMode)
    {
        int rate_control_id = rateControl->getSelectedId();
        int width = 0;
        if(rate_control_id==1)
        {
            if(isAdvancedMode)
                g.drawFittedText (LABEL_KB_PER_SECOND,
                                  200-48 + 187, 208+ upDetailed+160+ add, 45, 24,2,
                                  Justification::centred, true);
        }
        String text;
        switch(rate_control_id)
        {
        case 1:
            text = LABEL_VIDEO_SAVE_BITRATE;
            break;
        case 2:
            text = LABEL_VIDEO_SAVE_RATE_CONTROL_COEFF_CQP;
            break;
        case 3:
            text = LABEL_VIDEO_SAVE_RATE_CONTROL_COEFF_CRF;
            break;
        }

        g.drawFittedText (text,
                          20, 204+ upDetailed+160+add, 148-20-20, 30,2,
                          Justification::centredRight, true);


        g.drawFittedText (LABEL_VIDEO_SAVE_RATE_CONTROL,
                          0, 204+ upDetailed+120+add, 148-20, 30,2,
                          Justification::centredRight, true);


        g.drawFittedText (LABEL_VIDEO_SAVE_RESOLUTION,
                          0, 164+ upDetailed+120 + add, 148-20, 30,2,
                          Justification::centredRight, true);

    }
    g.drawFittedText (LABEL_VIDEO_SAVE_CODEC,
                      0, 124+ upDetailed, 148-20, 30,2,
                      Justification::centredRight, true);



    g.drawFittedText (LABEL_VIDEO_SAVE_SIZE,
                      0, 164+ upDetailed, 148-20, 30,2,
                      Justification::centredRight, true);


    g.drawFittedText (LABEL_VIDEO_SAVE_QUALITY,
                      0, 204+ upDetailed, 148-20, 30,2,
                      Justification::centredRight, true);

    if(hasCompressionPreset)
        g.drawFittedText (LABEL_VIDEO_SAVE_COMPRESSION,
                          0, 204+ upDetailed + 40, 148-20, 30,2,
                          Justification::centredRight, true);



    if(isAdvancedMode)
        g.drawFittedText (LABEL_VIDEO_SAVE_FPS,
                          0, 244+ upDetailed+160 + add, 148-20, 30,2,
                          Justification::centredRight, true);


    if(isAdvancedMode)
        g.drawText (T("X"),
                    300-28-20, 164+ upDetailed+120 + add, 32, 30,
                    Justification::centred, true);


    if(isAdvancedMode)
        g.drawFittedText (LABEL_VIDEO_SAVE_PASS,
                          0, 324+ upDetailed+160 + add, 148-20, 30,2,
                          Justification::centredRight, true);

    if(isAdvancedMode)
        g.drawFittedText (LABEL_VIDEO_GOP,
                          0, 284+ upDetailed+160 + add, 148-20, 30,2,
                          Justification::centredRight, true);

    /* ~video labels */
    /* audio labels */
    g.setColour (Colours::black);


    if(!isAudioEnabled)
        g.setColour (Colours::grey);

    g.drawFittedText (LABEL_AUDIO_SAVE_CODEC,
                      420-20, 124+ upDetailed, 108, 30,2,
                      Justification::centredRight, true);

    g.drawFittedText (LABEL_AUDIO_SAVE_BITRATE,
                      420-20, 164+ upDetailed, 108, 30,2,
                      Justification::centredRight, true);


    g.drawFittedText (LABEL_AUDIO_SAVE_SAMPLE_RATE,
                      420-20, 204+ upDetailed, 108, 30,2,
                      Justification::centredRight, true);



    g.drawFittedText (LABEL_AUDIO_SAVE_CHANNELS,
                      420-20, 244+ upDetailed, 108, 30,2,
                      Justification::centredRight, true);



    g.drawFittedText (LABEL_KB_PER_SECOND,
                      535-20 + 202, 168+ upDetailed, 45, 24,2,
                      Justification::centred, true);

    g.drawFittedText ("Hz",
                      535-20 + 202, 208+ upDetailed, 45, 24,2,
                      Justification::centred, true);

    /* ~audio labels */


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
    format->setBounds (232, 48, 540, 24);
    path->setBounds (232, 8, 540, 24);
    int group_height = 224+120+40 + 40;
    if(!isAdvancedMode)
    {
        group_height -= 200 + 40;
    }
    int add = 0;
    if(hasCompressionPreset)
        add = 40;


    enableVideo->setBounds (16+20, 104+ upDetailed-40, 360, 40);
    groupComponent->setBounds (16, 104+ upDetailed , 380, group_height + add );
    videoCodec->setBounds (200-48, 128+ upDetailed, 232, 24);
    videoWidth->setBounds (200-48, 168+ upDetailed+120 + add, 88, 24);
    videoHeight->setBounds (344-48, 168+ upDetailed+120 + add, 88, 24);
    rateControl->setBounds (200-48, 208+ upDetailed+120 + add, 232, 24);
    videoBitrate->setBounds (200-48, 208+ upDetailed+160 + add, 182, 24);
    crf->setBounds (200-48, 208+ upDetailed+160 + add, 232, 24);
    fps->setBounds (200-48, 248+ upDetailed+160 + add, 232, 24);
    gop->setBounds (200-48, 288+ upDetailed+160 + add, 232, 24);
    resolutionList->setBounds (200-48, 128+ upDetailed+40, 232, 24);
    qualityList->setBounds (200-48, 128+ upDetailed+80, 232, 24);
    advancedMode->setBounds (200-48, 128+ upDetailed+120 + add, 232, 24);
    passList->setBounds (200-48, 288+ upDetailed+160+40+ add, 232, 24);
    enableAudio->setBounds (400+20, 104+ upDetailed-40, 360, 40);
    groupComponent2->setBounds (400, 104+ upDetailed, 380, 184);
    audioCodec->setBounds (535-20, 128+ upDetailed, 252, 24);
    audioBitrate->setBounds (535-20, 168+ upDetailed, 202, 24);
    audioSampleRate->setBounds (535-20, 208+ upDetailed, 202, 24);
    channels->setBounds (535-20, 248+ upDetailed, 252, 24);
    ok->setBounds (72+380, 300 + upDetailed, 150, 24);
    cancel->setBounds (248+380, 300 + upDetailed, 150, 24);
    compressionPreset->setBounds (200-48, 128+ upDetailed+120, 232, 24);
}
void encodeVideoComponent::filenameComponentChanged (FilenameComponent* fileComponentThatHasChanged)
{
    if(fileComponentThatHasChanged == path)
    {
        UpdateFileExtension(false);
    }
}
void encodeVideoComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == rateControl)
    {
        int selected_id = rateControl->getSelectedId();
        if(selected_id>1)
        {
            passList->setEnabled(false);
            crf->setVisible(true);
            videoBitrate->setVisible(false);
        }
        else
        {
            crf->setVisible(false);
            videoBitrate->setVisible(true);
            passList->setEnabled(true);
        }
        recalculateCRF();
        recalculateBitrate();

        resized();
        repaint();

    }
    else if (comboBoxThatHasChanged == format)
    {
        /* set file extension */
        UpdateFileExtension(true);
        capabilities::Format selected_format = capabilities::formats.at(format->getSelectedId()-1);
        /* ~set file extension */

        /* set aviable codecs */
        String previous_id_string = String::empty;
        if(videoCodec->getSelectedId()-1>=0&&videoCodec->getSelectedId()-1<capabilities::video_codecs.size())
            previous_id_string = capabilities::video_codecs[videoCodec->getSelectedId()-1].id;
        videoCodec->clear(true);
        int previous_id = 1;
        vector<capabilities::VideoCodec*> codecs = selected_format.getCodecs();
        int id = 0;
        for(vector<capabilities::VideoCodec*>::iterator it = codecs.begin(); it!=codecs.end(); ++it)
        {
            videoCodec->addItem((*it)->display_id + "," + String((*it)->description),++id);
            if(previous_id_string == (*it)->id)
            {
                previous_id = id;
            }
        }

        videoCodec->setSelectedId(previous_id);

        /* ~set aviable codecs */
        /* allow video and audio */
        if(!selected_format.allowVideo)
        {
            enableVideo->setEnabled(false);
            enableVideo->setToggleState(false,true);

            enableAudio->setEnabled(false);
            enableAudio->setToggleState(true,true);
        }
        else if(!selected_format.allowAudio)
        {
            enableAudio->setEnabled(false);
            enableAudio->setToggleState(false,true);

            enableVideo->setEnabled(false);
            enableVideo->setToggleState(true,true);
        }
        else
        {
            enableVideo->setEnabled(true);
            enableAudio->setEnabled(true);
        }

        /* ~allow video and audio */
    }
    else if (comboBoxThatHasChanged == qualityList)
    {
        recalculateCRF();
        recalculateBitrate();

    }
    else if (comboBoxThatHasChanged == videoCodec)
    {
        capabilities::Format &current_format = capabilities::formats[format->getSelectedId()-1];
        capabilities::VideoCodec vc = capabilities::video_codecs[videoCodec->getSelectedId()-1];
        vector<capabilities::ResolutionPreset> resolutions = vc.getResolutions(current_format);
        int selected_id = resolutionList->getSelectedId();
        bool selected_id_present = false;
        resolutionList->clear(true);
        resolutionList->addItem(LABEL_VIDEO_SAVE_AUTO,1);
        for(vector<capabilities::ResolutionPreset>::iterator it = resolutions.begin(); it!=resolutions.end(); ++it)
        {
            int gcd = toolbox::GCD<int>(it->width,it->height);
            String resString = String(it->width) + String(" x ") + String(it->height) + String(" (")+ String(it->width/gcd) + String(":")+ String(it->height/gcd) + String(") ");
            if(it->tag!=String::empty)
            {
                resString = it->tag + "  " + resString;
            }
            int new_selected_id = it->toInt32();
            if(new_selected_id==selected_id)
                selected_id_present = true;
            resolutionList->addItem(resString,new_selected_id);
        }
        resolutionList->addItem(LABEL_VIDEO_SAVE_SIZE_OTHER,-100);
        if(selected_id_present)
            resolutionList->setSelectedId(selected_id);
        else
            resolutionList->setSelectedId(1);

        /* Update gui: change CompressionPreset visibility */
        bool needToHaveCompressionPreset = vc.hasCompressionPreset();
        if(needToHaveCompressionPreset != hasCompressionPreset)
        {
            int new_height_parent = getParentComponent()->getHeight();
            int new_height = getHeight();
            if(needToHaveCompressionPreset && !hasCompressionPreset)
            {
                if(isAdvancedMode)
                {
                    new_height_parent += 40;
                    new_height += 40;
                }
                compressionPreset->setVisible(true);

            }
            else
            {
                if(isAdvancedMode)
                {
                    new_height_parent -= 40;
                    new_height -= 40;
                }
                compressionPreset->setVisible(false);

            }
            hasCompressionPreset = needToHaveCompressionPreset;
            if(!isAdvancedMode)
                resized();
            setSize(getWidth(),new_height);
            getParentComponent()->setSize(getParentComponent()->getWidth(),new_height_parent);
            repaint();
        }
        /* ~Update gui: change CompressionPreset visibility */

        /*  Update gui: two pass */
        if(vc.allowTwoPass)
        {
            passList->setEnabled(true);
        }
        else
        {
            passList->setSelectedId(1);
            passList->setEnabled(false);
        }

        /*  ~Update gui: two pass */

        /* Update gop */
        if(!gopSetByUser)
        {
            if(vc.hasCompressionPreset())
                gop->setText(String("250"),false);
            else
                gop->setText(String("12"),false);
        }
        /* ~Update gop */
        /* Update Rate Control */
        int rate_control_id = rateControl->getSelectedId();
        int rate_control_selected_id = 1;
        rateControl->clear();
        rateControl->addItem(LABEL_VIDEO_SAVE_RATE_CONTROL_BITRATE,1);
        if(vc.hasCompressionPreset())
        {
            if(rate_control_id>1)
                rate_control_selected_id = 3;
            rateControl->addItem(LABEL_VIDEO_SAVE_RATE_CONTROL_CRF,3);
        }
        else if(vc.canCQ())
        {
            if(rate_control_id>1)
                rate_control_selected_id = 2;
            rateControl->addItem(LABEL_VIDEO_SAVE_RATE_CONTROL_CQ,2);
        }
        rateControl->setSelectedId(rate_control_selected_id,true);
        /* ~Update Rate Control */

    }
    else if (comboBoxThatHasChanged == resolutionList)
    {
        int id = resolutionList->getSelectedId();
        if(id==-100)
        {
            if(!isAdvancedMode)
                advancedMode->setToggleState(true,true);
            videoWidth->setSelectAllWhenFocused(true);
            videoWidth->grabKeyboardFocus();
            videoWidth->setSelectAllWhenFocused(false);
            return;
        }
        if(id==1)
        {
            int max_pixel = 0;
            int max_pixel_width = 0;
            int max_pixel_height = 0;
            for(vector<Timeline::Interval*>::iterator it = mainWindow->timeline->intervals.begin(); it!=mainWindow->timeline->intervals.end(); ++it)
            {
                int current_pixel_width = (*it)->movie->width;
                int current_pixel_height = (*it)->movie->height;

                int current_pixel = current_pixel_width * current_pixel_height;
                if(current_pixel > max_pixel)
                {
                    max_pixel_width = current_pixel_width;
                    max_pixel_height = current_pixel_height;
                    max_pixel = current_pixel;
                }
            }

            videoWidth->setText(String(max_pixel_width),false);
            videoHeight->setText(String(max_pixel_height),false);
        }
        else
        {
            capabilities::ResolutionPreset resolution;
            resolution.fromInt32(resolutionList->getSelectedId());
            videoWidth->setText(String(resolution.width),false);
            videoHeight->setText(String(resolution.height),false);
        }
        comboBoxChanged(qualityList);
    }
    else if (comboBoxThatHasChanged == audioCodec)
    {

    }
}
void encodeVideoComponent::clearValidation()
{
    fps->setColour(TextEditor::textColourId,Colour::fromRGB(0,0,0));
    fps->applyFontToAllText(fps->getFont());
    fps->setTooltip("");
    videoWidth->setColour(TextEditor::textColourId,Colour::fromRGB(0,0,0));
    videoWidth->applyFontToAllText(videoWidth->getFont());
    videoWidth->setTooltip("");
    videoHeight->setColour(TextEditor::textColourId,Colour::fromRGB(0,0,0));
    videoHeight->applyFontToAllText(videoHeight->getFont());
    videoHeight->setTooltip("");

    crf->setColour(TextEditor::textColourId,Colour::fromRGB(0,0,0));
    crf->applyFontToAllText(crf->getFont());
    crf->setTooltip("");
    videoBitrate->setColour(TextEditor::textColourId,Colour::fromRGB(0,0,0));
    videoBitrate->applyFontToAllText(videoBitrate->getFont());
    videoBitrate->setTooltip("");
}

bool encodeVideoComponent::Validate()
{
    bool res = true;

    double fps_double = fps->getText().getDoubleValue();
    if(fps_double>60.0 || fps_double<5.0)
    {
        fps->setColour(TextEditor::textColourId,Colour::fromRGB(255,0,0));
        fps->setTooltip(VALIDATION_SAVE_FPS);
        fps->applyFontToAllText(fps->getFont());
        res = false;
    }

    int width_int = videoWidth->getText().getIntValue();
    if(width_int<5)
    {
        videoWidth->setColour(TextEditor::textColourId,Colour::fromRGB(255,0,0));
        videoWidth->setTooltip(VALIDATION_SAVE_WIDTH);
        videoWidth->applyFontToAllText(videoWidth->getFont());
        res = false;
    }

    int height_int = videoHeight->getText().getIntValue();
    if(height_int<5)
    {
        videoHeight->setColour(TextEditor::textColourId,Colour::fromRGB(255,0,0));
        videoHeight->setTooltip(VALIDATION_SAVE_HEIGHT);
        videoHeight->applyFontToAllText(videoHeight->getFont());
        res = false;
    }

    if(rateControl->getSelectedId()==1)
    {
        int video_bitrate_int = videoBitrate->getText().getIntValue();
        if(video_bitrate_int==0)
        {
            videoBitrate->setColour(TextEditor::textColourId,Colour::fromRGB(255,0,0));
            videoBitrate->setTooltip(VALIDATION_SAVE_BITRATE);
            videoBitrate->applyFontToAllText(videoBitrate->getFont());
            res = false;
        }
    }
    else
    {
        int crf_int = crf->getText().getIntValue();
        capabilities::VideoCodec vc = capabilities::video_codecs[videoCodec->getSelectedId()-1];
        if(crf_int<vc.qmin || crf_int>vc.qmax)
        {
            crf->setColour(TextEditor::textColourId,Colour::fromRGB(255,0,0));
            crf->setTooltip(VALIDATION_SAVE_CRF_MAIN + String(vc.qmin) + VALIDATION_SAVE_CRF_TO + String(vc.qmax));

            crf->applyFontToAllText(crf->getFont());
            res = false;
        }
    }

    return res;
}

void encodeVideoComponent::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == ok)
    {
        clearValidation();
        if(!Validate())
        {
            if(!isAdvancedMode)
                advancedMode->setToggleState(true,true);
            return;
        }


        mainWindow->timeline->Render(GetMovieInfo());
        getParentComponent()->removeFromDesktop();
        path->setCurrentFile(getCurrentFileName(),false);
        UpdateFileExtension(true);
    }
    else if (buttonThatWasClicked == cancel)
    {
        getParentComponent()->removeFromDesktop();
    }
    else if (buttonThatWasClicked == enableVideo)
    {
        isVideoEnabled = !isVideoEnabled;
        if(isVideoEnabled)
        {
            videoWidth->setEnabled(true);
            videoHeight->setEnabled(true);
            videoCodec->setEnabled(true);
            videoBitrate->setEnabled(true);
            fps->setEnabled(true);
            gop->setEnabled(true);
            advancedMode->setEnabled(true);
            passList->setEnabled(true);
            rateControl->setEnabled(true);
            qualityList->setEnabled(true);
            compressionPreset->setEnabled(true);
            resolutionList->setEnabled(true);
        }
        else
        {
            videoWidth->setEnabled(false);
            videoHeight->setEnabled(false);
            videoCodec->setEnabled(false);
            videoBitrate->setEnabled(false);
            fps->setEnabled(false);
            gop->setEnabled(false);
            rateControl->setEnabled(false);
            advancedMode->setEnabled(false);
            passList->setEnabled(false);
            qualityList->setEnabled(false);
            compressionPreset->setEnabled(false);
            resolutionList->setEnabled(false);
            advancedMode->setToggleState(false,true);
        }

        repaint();
        if(!isVideoEnabled && !isAudioEnabled)
        {
            enableAudio->setToggleState(true,true);
        }

    }
    else if (buttonThatWasClicked == enableAudio)
    {
        isAudioEnabled = !isAudioEnabled;
        if(isAudioEnabled)
        {
            audioBitrate->setEnabled(true);
            audioCodec->setEnabled(true);
            audioSampleRate->setEnabled(true);
            channels->setEnabled(true);
        }
        else
        {
            audioBitrate->setEnabled(false);
            audioCodec->setEnabled(false);
            audioSampleRate->setEnabled(false);
            channels->setEnabled(false);
        }
        repaint();

        if(!isVideoEnabled && !isAudioEnabled)
        {
            enableVideo->setToggleState(true,true);
        }


    }
    else if (buttonThatWasClicked == advancedMode)
    {
        isAdvancedMode = !isAdvancedMode;
        rateControl->setVisible(isAdvancedMode);
        bool is_crf = rateControl->getSelectedId()>1;
        crf->setVisible(isAdvancedMode && is_crf);
        videoBitrate->setVisible(isAdvancedMode && !is_crf);
        gop->setVisible(isAdvancedMode);
        fps->setVisible(isAdvancedMode);
        videoWidth->setVisible(isAdvancedMode);
        videoHeight->setVisible(isAdvancedMode);
        passList->setVisible(isAdvancedMode);
        int new_height = getHeight();
        int new_height_parent = getParentComponent()->getHeight();
        if(isAdvancedMode)
        {
            if(hasCompressionPreset)
            {
                new_height+=40;
                new_height_parent+=40;
            }
            new_height+=200;
            new_height_parent+=200;
        }
        else
        {
            if(hasCompressionPreset)
            {
                new_height-=40;
                new_height_parent-=40;
            }
            new_height-=200;
            new_height_parent-=200;
        }
        setSize(getWidth(),new_height);
        getParentComponent()->setSize(getParentComponent()->getWidth(),new_height_parent);
    }


}

