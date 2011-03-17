#include "soundOutput.h"
SineWaveSound::SineWaveSound(){}
bool SineWaveSound::appliesToNote (const int /*midiNoteNumber*/){return true;}
bool SineWaveSound::appliesToChannel (const int /*midiChannel*/){return true;}

SineWaveVoice::SineWaveVoice() : angleDelta (0.0), tailOff (0.0)
{
}

bool SineWaveVoice::canPlaySound (SynthesiserSound* sound)
{
    return dynamic_cast <SineWaveSound*> (sound) != 0;
}

void SineWaveVoice::startNote (const int midiNoteNumber, const float velocity, SynthesiserSound* /*sound*/, const int /*currentPitchWheelPosition*/)
{
    currentAngle = 0.0;
    level = velocity * 0.15;
    tailOff = 0.0;

    double cyclesPerSecond = MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    double cyclesPerSample = cyclesPerSecond / getSampleRate();

    angleDelta = cyclesPerSample * 2.0 * double_Pi;
}

void SineWaveVoice::stopNote (const bool allowTailOff)
{
    if (allowTailOff)
    {
        // start a tail-off by setting this flag. The render callback will pick up on
        // this and do a fade out, calling clearCurrentNote() when it's finished.

        if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
            // stopNote method could be called more than once.
            tailOff = 1.0;
    }
    else
    {
        // we're being told to stop playing immediately, so reset everything..

        clearCurrentNote();
        angleDelta = 0.0;
    }
}

void SineWaveVoice::pitchWheelMoved (const int /*newValue*/)
{
    // can't be bothered implementing this for the demo!
}

void SineWaveVoice::controllerMoved (const int /*controllerNumber*/, const int /*newValue*/)
{
    // not interested in controllers in this case.
}

void SineWaveVoice::renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (angleDelta != 0.0)
    {
        if (tailOff > 0)
        {
            while (--numSamples >= 0)
            {
                const float currentSample = (float) (sin (currentAngle) * level * tailOff);

                for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                    *outputBuffer.getSampleData (i, startSample) += currentSample;

                currentAngle += angleDelta;
                ++startSample;

                tailOff *= 0.99;

                if (tailOff <= 0.005)
                {
                    clearCurrentNote();

                    angleDelta = 0.0;
                    break;
                }
            }
        }
        else
        {
            while (--numSamples >= 0)
            {
                const float currentSample = (float) (sin (currentAngle) * level);

                for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                    *outputBuffer.getSampleData (i, startSample) += currentSample;

                currentAngle += angleDelta;
                ++startSample;
            }
        }
    }
}

SineWaveVoice voice;

SynthAudioSource::SynthAudioSource ()
{

    synth.addVoice (&voice);

    synth.clearSounds();
    synth.addSound (new SineWaveSound());
    //synth.setCurrentPlaybackSampleRate (1000);
}


void SynthAudioSource::releaseResources()
{
}

void SynthAudioSource::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    MidiBuffer incomingMidi;
    synth.renderNextBlock (*bufferToFill.buffer,incomingMidi, 0, bufferToFill.numSamples);
}
void SynthAudioSource::prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
}
AudioDeviceManager sound_manager;
AudioSourcePlayer audioSourcePlayer;
SynthAudioSource synthAudioSource;

void init_sound()
{
    sound_manager.initialise (2, 2, 0, true, String::empty, 0);
    //sound_manager.playTestSound();
    audioSourcePlayer.setSource (&synthAudioSource);
    sound_manager.addAudioCallback (&audioSourcePlayer);

    //voice.startNote(80,1.0,0,0);

}

void close_sound()
{
    audioSourcePlayer.setSource (0);
    sound_manager.removeAudioCallback (&audioSourcePlayer);
}

