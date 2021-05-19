/*
  ==============================================================================

    ModulationSystem.h
    Created: 1 May 2021 11:02:04am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "DAHDSR.h"
#include "LFO.h"
#include "Filter.h"
//! macros to define what parameters should be created
#define NUM_OSCILLATORS 4
#define NUM_LFOS 4
#define NUM_VOICES 6
//! constants for scaling
#define PITCHWHEEL_RANGE 16384
#define MIDI_NOTES 128
#define UNISON_MAX 4 //! limit to 4 unison voices above and 4 below

class SynthParam;

struct ModSource //represents a modulation source
{
    ModSource(SynthParam* source, float d=1.0f) : src(source), depth(d) {}
    SynthParam* const src;
    float depth;
};

class SynthParam //represents a mod destination/ parameter that needs to be continuously updated
{
public:
    SynthParam(juce::String n, float lo=0.0f, float hi=1.0f, float normal=0.5f) :
    name(n),
    min(lo),
    max(hi),
    currentBaseValue(normal),
    targetBaseValue(normal),
    maxSampleDelta((max - min) / 3000.0f),
    paramColor(juce::Colours::lightblue),
    hasChosenColor(false)
    {
    }
    virtual ~SynthParam()
    {
    }
    std::vector<ModSource*> modSources;
    const juce::String name;
    void tickValue();
    virtual float getActual(int voiceIndex=0) {return 0.0f; }
    float getAdjusted(int voice = 0); //the value normalized to the range 0 - 1
    ModSource* makeSource(float d);
    virtual void setBase(float val) //for setting the value from a silder or similar
    {
        targetBaseValue = val;
    }
    void setDierct(float val) //set the value directly and bypass parameter smoothing
    {
        setBase(val);
        currentBaseValue = targetBaseValue;
    }
    float getFloatAdj()
    {return currentBaseValue / (max - min); }
    float getBaseValue() {return currentBaseValue; }
    float getMin() {return min; }
    float getMax() {return max; }
    void addSource(ModSource* newSrc) { modSources.push_back(newSrc); }
    void addSource(SynthParam* src, float depth = 1.0f) {modSources.push_back(src->makeSource(depth)); }
    void removeSource(ModSource* toRemove);
    void removeSource(int index)
    {
        modSources.erase(modSources.begin() + index);
    }
    juce::Colour getColor() {return paramColor; }
protected:
    float min, max;
    float currentBaseValue;
    float targetBaseValue;
    float maxSampleDelta;
    float actualOffset(ModSource* mod, int index=0);
    
    float actualOut;
    float adjOut;
    juce::Colour paramColor;
    bool hasChosenColor;
    bool isVoiceDependent;
};
//=============================================================================================================
class GlobalTargetParam : public SynthParam
{
public:
    GlobalTargetParam(juce::String n, float lo=0.0f, float hi=1.0f, float normal=0.5f) :
    SynthParam(n, lo, hi, normal)
    {
        
    }
    float getActual(int voice=0) override
    {
        tickValue();
        actualOut = currentBaseValue;
        for(auto src : modSources)
        {
            actualOut += actualOffset(src);
        }
        return actualOut;
    }
    void setBase(float newVal) override
    {
        targetBaseValue = newVal;
    }
};

//=============================================================================================================
class ContinuousVoiceParam : public SynthParam //! to be used for modulator outputs, represents the output of an LFO or envelope for example. must be updated every sample
{
public:
    ContinuousVoiceParam(juce::String n, float lo=0.0f, float hi=1.0f, float normal=0.5f) :
    SynthParam(n, lo, hi, normal)
    {
        for(auto val : voiceOutputs)
            val = normal;
    }
    void setOutput(int voiceIndex, float newValue)
    {
        //currentBaseValue = newValue;
        voiceOutputs[voiceIndex] = newValue;
    }
    float getActual(int voiceIndex=0) override
    {
        return voiceOutputs[voiceIndex];
    }
    void setBase(float val) override
    {
        currentBaseValue = val;
        for(auto v : voiceOutputs)
        {
            v = val;
        }
    }
private:
    std::array<float, NUM_VOICES> voiceOutputs;
};

class VoiceTargetParam : public SynthParam //! use for osc level, wt position, etc
{
public:
    VoiceTargetParam(juce::String n, float lo=0.0f, float hi=1.0f, float normal=0.5f) :
    SynthParam(n, lo, hi, normal)
    {
        for(int i = 0; i < NUM_VOICES; ++i)
        {
            voiceOffsets[i] = 0.0f;
        }
        //=============================
        /*
        printf("Parameter %s initial values\n", name.toRawUTF8());
        printf("Normal is %f\n", normal);
        for(int i = 0; i < NUM_VOICES; ++i)
        {
            printf("Output #%d: %f\n", i, voiceOutputs[i]);
            printf("Offest #%d: %f\n", i, voiceOffsets[i]);
        }
        printf("\n");
         */
    }
    void tickModulation(int voice)
    {
        voiceOffsets[voice] = 0.0f;
        for(auto src : modSources)
        {
            voiceOffsets[voice] += actualOffset(src, voice);
        }
    }
    float getActual(int voiceIndex) override
    {
        tickModulation(voiceIndex);
        return currentBaseValue + voiceOffsets[voiceIndex];
    }
    void setBase(float val) override
    {
        currentBaseValue = val;
    }
private:
    std::array<float, NUM_VOICES> voiceOffsets;
};

class MacroSourceParam : public SynthParam //! for things like pitch wheel, MIDI cc, etc
{
public:
    MacroSourceParam(juce::String n, float lo=0.0f, float hi=1.0f, float normal=0.0f) :
    SynthParam(n, lo, hi, normal)
    {
        
    }
    float getActual(int voiceIndex) override
    {
        return currentBaseValue;
    }
    void setBase(float value) override
    {
        currentBaseValue = value;
    }
};

class SynthParameterGroup //! this class should be instantiated once and each voice should update via a pointer to it
{
public:
    SynthParameterGroup(juce::AudioProcessorValueTreeState* t);
    using paramVecGTarget = juce::OwnedArray<GlobalTargetParam>;
    using paramVecCont = juce::OwnedArray<ContinuousVoiceParam>;
    using paramVecTarget = juce::OwnedArray<VoiceTargetParam>;
    using paramVecPtr = juce::OwnedArray<GlobalTargetParam>*;
    using paramPowerVec = juce::OwnedArray<MacroSourceParam>;
    using apvts = juce::AudioProcessorValueTreeState;
    using floatParam = juce::AudioParameterFloat;
    using fRange = juce::NormalisableRange<float>;
    void updateForBlock(apvts& tree);
    apvts::ParameterLayout createLayout();
    paramVecGTarget mDelays;
    paramVecGTarget aDelays;
    paramVecGTarget mAttacks;
    paramVecGTarget aAttacks;
    paramVecGTarget mHolds;
    paramVecGTarget aHolds;
    paramVecGTarget mDecays;
    paramVecGTarget aDecays;
    paramVecGTarget mSustains;
    paramVecGTarget aSustains;
    paramVecGTarget mReleases;
    paramVecGTarget aReleases;
    paramVecTarget oscPositions;
    paramVecTarget oscLevels;
    paramVecTarget oscPans;
    paramPowerVec oscUnisonModes;
    paramVecGTarget oscUnisonVoices;
    paramVecGTarget oscUnisonSpreads;
    paramVecGTarget oscUnisonLevels;
    paramVecGTarget lfoRates;
    paramVecGTarget lfoRetriggers;
    paramVecCont oscAmpEnvs;
    paramVecCont oscModEnvs;
    paramVecCont lfoOutputs;
    paramPowerVec oscPowers;
    paramPowerVec lfoPowers;
    VoiceTargetParam filterCutoff;
    VoiceTargetParam filterResonance;
    VoiceTargetParam filterWetDry;
    MacroSourceParam pitchWheelValue;
    MacroSourceParam modWheelValue;
    ContinuousVoiceParam keyTrackValue;
    MacroSourceParam lastTriggeredVoice;
    apvts* const linkedTree;
    std::array<std::vector<std::vector<float>>, NUM_OSCILLATORS> oscGraphVectors;
    std::array<lfoArray, NUM_LFOS> lfoShapes;
};



