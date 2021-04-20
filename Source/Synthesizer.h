/*
  ==============================================================================

    Synthesizer.h
    Created: 19 Apr 2021 4:21:38pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "WavetableProcessor.h"
#define NUM_VOICES 6

class WavetableSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int /*midiNoteNumber*/) //just plays this sound for any midi note
    {
        return true;
    }
    bool appliesToChannel(int /*midiChannel*/) //plays the sound on both channels
    {
        return true;
    }
};

class WavetableVoice : public juce::SynthesiserVoice
{
public:
    WavetableVoice(juce::File& file);
    void pitchWheelMoved(int newPitchWheelVal) override {}
    //=============================================
    void controllerMoved(int controllerNumber, int controllerValue) override {}
    //===============================================
    void startNote (int midiNoteNumber,
                    float velocity,
                    juce::SynthesiserSound *sound,
                    int currentPitchWheelPosition) override;
    //===============================================
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<WavetableSound*>(sound) != nullptr;
    }
    void stopNote (float velocity, bool allowTailOff) override;
    //==============================================
    void aftertouchChanged (int newAftertouchValue) override {}
    //==============================================
    void channelPressureChanged (int newChannelPressureValue) override {}
    //===============================================
    void renderNextBlock (juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    //================================================
    void setSampleRate(double newRate);
    void updateParams(juce::AudioProcessorValueTreeState* t);
    void updateEnvelope();
    double fundamental;
    WavetableOscHolder osc;
    DAHDSR env;
    float lastOutput;
    SynthParam pDelay;
    SynthParam pAttack;
    SynthParam pHold;
    SynthParam pDecay;
    SynthParam pSustain;
    SynthParam pRelease;
};

//! this class should be a public member of the AudioProcessor and so needs to handle all APVTS attachments and give any relevant data to the component side
class WavetableSynth : public juce::Synthesiser
{
public:
    WavetableSynth(juce::AudioProcessorValueTreeState* t);
    void replaceOscillators(juce::File newFile);
    void setSampleRate(double newRate);
    juce::File& getWaveFolder()
    {
        return waveFolder;
    }
    void updateFromTree();
    std::vector<std::vector<float>> getDataToGraph();
    std::vector<WavetableVoice*> WTvoices;
private:
    juce::File waveFolder;
    juce::File lastOscFile;
    int lastFileIdx;
    juce::AudioProcessorValueTreeState* tree;
};
