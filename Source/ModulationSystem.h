/*
  ==============================================================================

    ModulationSystem.h
    Created: 1 May 2021 11:02:04am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

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
    float baseValue;
    float actualOut;
    float adjOut;
};

class SynthParamStructure //represents the whole state of the synth
{
public:
    SynthParamStructure();
    SynthParam* getParamPtr(juce::String paramName);
    void saveToValueTree(juce::ValueTree* tree);
    void loadFromValueTree(juce::ValueTree* tree);
    void updateForBlock(juce::AudioProcessorValueTreeState& tree);
private:
    juce::OwnedArray<SynthParam> allParams;
};
