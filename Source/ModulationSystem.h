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
//macros to define what parameters should be created
#define NUM_OSCILLATORS 4
#define NUM_LFOS 4
#define NUM_VOICES 6

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
    float getAdjusted(); //the value normalized to the range 0 - 1
    ModSource* makeSource(float d);
    void setBase(float val) //for setting the value from a silder or similar
    {
        targetBaseValue = val;
    }
    void setDierct(float val) //set the value directly and bypass parameter smoothing
    {
        setBase(val);
        currentBaseValue = targetBaseValue;
    }
    float getBaseValue() {return currentBaseValue; }
    float getMin() {return min; }
    float getMax() {return max; }
    void addSource(ModSource* newSrc) { modSources.push_back(newSrc); }
    void addSource(SynthParam* src, float depth = 1.0f) {modSources.push_back(src->makeSource(depth)); }
    void removeSource(ModSource* toRemove);
    juce::Colour getColor() {return paramColor; }
protected:
    float min, max;
    float currentBaseValue;
    float targetBaseValue;
    float maxSampleDelta;
    float actualOffset(ModSource* mod);
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
};

//=============================================================================================================
class ContinuousVoiceParam : public SynthParam
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
        voiceOutputs[voiceIndex] = newValue;
    }
    float getActual(int voiceIndex=0) override
    {
        return voiceOutputs[voiceIndex];
    }
private:
    std::array<float, NUM_VOICES> voiceOutputs;
};

class SynthParameterGroup //this class should be instantiated once and each voice should update via a pointer to it
{
public:
    SynthParameterGroup(juce::AudioProcessorValueTreeState* t);
    using paramVecGTarget = juce::OwnedArray<GlobalTargetParam>;
    using paramVecCont = juce::OwnedArray<ContinuousVoiceParam>;
    using paramVecPtr = juce::OwnedArray<GlobalTargetParam>*;
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
    paramVecGTarget oscPositions;
    paramVecGTarget oscLevels;
    paramVecCont oscAmpEnvs;
    paramVecCont oscModEnvs;
    apvts* const linkedTree;
    std::array<std::vector<std::vector<float>>, NUM_OSCILLATORS> oscGraphVectors;
};



