/*
  ==============================================================================

    Synthesizer.h
    Created: 30 Apr 2021 3:59:28pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Wavetable.h"
#include "DAHDSR.h"

#define NUM_VOICES 4

class OctaneSound : public juce::SynthesiserSound
{
    bool appliesToNote(int /*midiNoteNumber*/) //just plays this sound for any midi note
    {
        return true;
    }
    bool appliesToChannel(int /*midiChannel*/) //plays the sound on any midi channel
    {
        return true;
    }
};

class OctaneVoice : public juce::SynthesiserVoice
{
public:
    OctaneVoice(juce::File defaultTable, int idx);
    ~OctaneVoice();
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<OctaneSound*>(sound) != nullptr;
    }
    void stopNote (float velocity, bool allowTailOff) override;
    void startNote (int midiNoteNumber,
                    float velocity,
                    juce::SynthesiserSound *sound,
                    int currentPitchWheelPosition) override;
    void pitchWheelMoved(int newPitchWheelVal) override {} 
    void controllerMoved(int controllerNumber, int controllerValue) override {}
    void setAllSampleRate(double rate)
    {
        osc.setSampleRate(rate);
        env.setSampleRate(rate);
        setCurrentPlaybackSampleRate(rate);
    }
    //===============================================
    void aftertouchChanged (int newAftertouchValue) override {}
    //==============================================
    void channelPressureChanged (int newChannelPressureValue) override{}
    //===============================================
    void renderNextBlock (juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
private:
    int voiceIndex;
    double fundamental;
    float position;
    OctaneOsc osc;
    DAHDSR env;
    float lastOutput;
    int sample;
};

class OctaneSynth : public juce::Synthesiser
{
public:
    OctaneSynth();
    void setAllSampleRates(double rate)
    {
        setCurrentPlaybackSampleRate(rate);
        for(auto v : oVoices)
            v->setAllSampleRate(rate);
    }
private:
    juce::File waveFolder;
    std::vector<OctaneVoice*> oVoices;
    
};
