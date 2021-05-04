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
#include "ModulationSystem.h"



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
    OctaneVoice(juce::Array<juce::File>& waveFolder, SynthParameterGroup* grp, int idx);
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
        for(auto o : oscillators)
            o->setSampleRate(rate);
        setCurrentPlaybackSampleRate(rate);
    }
    //===============================================
    void aftertouchChanged (int newAftertouchValue) override {}
    //==============================================
    void channelPressureChanged (int newChannelPressureValue) override{}
    //===============================================
    void renderNextBlock (juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    //===============================================
    void tickBlock(); //update the things that need to be updated once per buffer
    void tickSample(); //update the things that need continuous modulation
    SynthParameterGroup* const params;
    const int voiceIndex;
    int oscIndex;
    double fundamental;
    float position;
    std::vector<ContinuousVoiceParam*> ampOutputs;
    std::vector<ContinuousVoiceParam*> modOutputs;
    juce::OwnedArray<OctaneOsc> oscillators;
    float oscLevelSum;
    float lastOscLevel;
    float lastOutput;
    int sample;
};

class OctaneSynth : public juce::Synthesiser
{
public:
    OctaneSynth(juce::AudioProcessorValueTreeState* tree);
    void setAllSampleRates(double rate)
    {
        setCurrentPlaybackSampleRate(rate);
        for(auto v : oVoices)
            v->setAllSampleRate(rate);
    }
    int getNumWaves() {return waveFiles.size(); }
    SynthParameterGroup paramGroup;
private:
    juce::File waveFolder;
    juce::Array<juce::File> waveFiles;
    std::vector<OctaneVoice*> oVoices;
    
};
