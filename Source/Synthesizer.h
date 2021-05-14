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
#include "LFO.h"
#include "ModulationSystem.h"
#include "Filter.h"
#define BLOCKS_PER_UPDATE 15
//===========================================================================
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
//===========================================================================
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
        for(auto l : lfos)
            l->setSampleRate(rate);
        setCurrentPlaybackSampleRate(rate);
    }
    //===============================================
    void aftertouchChanged (int newAftertouchValue) override {}
    //==============================================
    void channelPressureChanged (int newChannelPressureValue) override{}
    //===============================================
    void renderNextBlock (juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    //===============================================
    void replaceWave(int index, juce::File newWave);
    void tickBlock(); //update the things that need to be updated once per buffer
    void tickSample(); //update the things that need continuous modulation
    void prepare(double rate, int samplesPerBlock, int numChannels) {filter.prepare(rate, samplesPerBlock, numChannels); }
    std::vector<juce::File> getOscFiles();
    SynthParameterGroup* const params;
    const int voiceIndex;
    int oscIndex;
    int lfoIndex;
    double fundamental;
    float position;
    std::vector<ContinuousVoiceParam*> ampOutputs;
    std::vector<ContinuousVoiceParam*> modOutputs;
    std::vector<ContinuousVoiceParam*> lfoOutputs;
    juce::OwnedArray<OctaneOsc> oscillators;
    juce::OwnedArray<OctaneLFO> lfos;
    OctaneFilter filter;
    float oscLevelSum;
    float lastOscLevel;
    float lastOutput;
    int sample;
    int blockIndex;
};
//===========================================================================
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
    void prepare(double rate, int samplesPerBlock, int numChannels)
    {
        for(auto v : oVoices)
        {
            v->prepare(rate, samplesPerBlock, numChannels);
        }
        
    }
    int getNumWaves() {return waveFiles.size(); }
    void replaceLfos(int index);
    void replaceWaves(int index, juce::File newWave);
    void updateGraphData()
    {
        int idx = 0;
        for(auto& osc : paramGroup.oscGraphVectors)
        {
            osc = oVoices[0]->oscillators[idx]->getGraphData(128);
            ++idx;
        }
    }
    SynthParameterGroup paramGroup;
    juce::File waveFolder;
    juce::Array<juce::File> waveFiles;
    juce::File fileForOsc(int index) {return activeFiles[index]; }
private:
    std::vector<juce::File> activeFiles;
    std::vector<OctaneVoice*> oVoices;
};
//===========================================================================
enum class ChangeType
{
    WaveChange,
    LfoChange
};

class OctaneChange
{
public:
    virtual ~OctaneChange() {}
    virtual void apply() {}
};

class OctaneLFOChange : public OctaneChange
{
public:
    OctaneLFOChange(OctaneSynth* synth, int lfoIndex);
    void apply() override;
private:
    OctaneSynth* const linkedSynth;
    const int index;
};

class OctaneWaveChange : public OctaneChange
{
public:
    OctaneWaveChange(OctaneSynth* synth, int oscIndex, juce::File newWave);
    void apply() override;
private:
    OctaneSynth* const linkedSynth;
    const int index;
    juce::File wave;
};
//========================================================================
class OctaneUpdater : public juce::AsyncUpdater
{
public:
    OctaneUpdater(OctaneSynth* synth);
    void tick();
    OctaneSynth* const linkedSynth;
    void handleAsyncUpdate() override;
    void stageLfoChange(int index);
    void stageWaveChange(int index, juce::File wave);
private:
    juce::OwnedArray<OctaneChange> stagedChanges;
    int blockIndex;
};
