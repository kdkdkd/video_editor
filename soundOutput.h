#ifndef SOUND_OUTPUT
#define SOUND_OUTPUT
#include "config.h"
#include "timeline.h"
#include "juce/juce.h"


class SineWaveSound : public SynthesiserSound
{
public:
    SineWaveSound();
    bool appliesToNote (const int /*midiNoteNumber*/);
    bool appliesToChannel (const int /*midiChannel*/);
};



class SineWaveVoice  : public SynthesiserVoice
{
public:
    Timeline *timeline;
    SineWaveVoice();
    bool canPlaySound (SynthesiserSound* sound);
    void startNote (const int midiNoteNumber, const float velocity, SynthesiserSound* /*sound*/, const int /*currentPitchWheelPosition*/);
    void stopNote (const bool allowTailOff);
    void pitchWheelMoved (const int /*newValue*/);
    void controllerMoved (const int /*controllerNumber*/, const int /*newValue*/);
    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples);
    void AddTimeline(Timeline *timeline);

private:
    double currentAngle, angleDelta, level, tailOff;
};


class SynthAudioSource  : public AudioSource
{
public:

    Synthesiser synth;

    SynthAudioSource ();

    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate);

    void releaseResources();


    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill);

};

class SoundConnector
{
    private :
        uint8_t *data;
        int length;
        int position_in;
        int position_out;
    public :
        SoundConnector();
        ~SoundConnector();
        void fill_buffer_in(uint8_t* data, int size);
        void fill_buffer_out(AudioSampleBuffer& outputBuffer,int startSample,int number);
};


extern SoundConnector sound_connector;
extern SineWaveVoice voice;
void close_sound();
void init_sound(Timeline * timeline);
void start_sound();
void stop_sound();


#endif
