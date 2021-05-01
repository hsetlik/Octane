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

class SynthParam;

struct ModSource //represents a modulation source
{
    ModSource(SynthParam* source, float d=1.0f) : src(source), depth(d) {}
    SynthParam* src;
    float depth;
};

class SynthParam //represents a mod destination/ parameter that needs to be continuously updated
{
public:
    SynthParam(juce::String n, float lo=0.0f, float hi=1.0f, float normal=0.5f) :
    name(n),
    min(lo),
    max(hi),
    baseValue(normal)
    {
        
    }
    juce::String name; //every parameter needs a distinct name
    std::vector<ModSource*> modSources;
    float min, max;
    float baseValue;
    float getActual();
    float getAdjusted(); //the value normalized to the range 0 - 1
    ModSource* makeSource(float d);
    void setBase(float val) //for setting the value from a silder or similar
    {
        baseValue = val;
    }
    void setBase(juce::AudioProcessorValueTreeState& tree)
    {
        baseValue = *tree.getRawParameterValue(name);
    }
    void addSource(ModSource* newSrc) { modSources.push_back(newSrc); }
    void addSource(SynthParam* src, float depth = 1.0f) {modSources.push_back(src->makeSource(depth)); }
protected:
    float actualOffset(ModSource* mod);
    float actualOut;
    float adjOut;
};

class SynthParameterGroup //this class should be instantiated once and each voice should update via a pointer to it
{
public:
    SynthParameterGroup();
    using paramVec = juce::OwnedArray<SynthParam>;
    using paramVecPtr = juce::OwnedArray<SynthParam>*;
    using apvts = juce::AudioProcessorValueTreeState;
    using floatParam = juce::AudioParameterFloat;
    using fRange = juce::NormalisableRange<float>;
    void updateForBlock(apvts& tree);
    apvts::ParameterLayout createLayout();
    paramVec mDelays;
    paramVec aDelays;
    paramVec mAttacks;
    paramVec aAttacks;
    paramVec mHolds;
    paramVec aHolds;
    paramVec mDecays;
    paramVec aDecays;
    paramVec mSustains;
    paramVec aSustains;
    paramVec mReleases;
    paramVec aReleases;
    paramVec oscPositions;
    std::vector<paramVecPtr> allVecs;
};



