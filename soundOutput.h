#ifndef SOUND_OUTPUT
#define SOUND_OUTPUT
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
    SineWaveVoice();
    bool canPlaySound (SynthesiserSound* sound);
    void startNote (const int midiNoteNumber, const float velocity, SynthesiserSound* /*sound*/, const int /*currentPitchWheelPosition*/);
    void stopNote (const bool allowTailOff);
    void pitchWheelMoved (const int /*newValue*/);
    void controllerMoved (const int /*controllerNumber*/, const int /*newValue*/);
    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples);

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

void init_sound();
void close_sound();




#endif
