/*
  ==============================================================================

    Filter.cpp
    Created: 13 May 2021 3:41:25pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Filter.h"
std::vector<juce::String> OctaneFilter::FilterNames{"LoPass12", "LoPass24", "HiPass12", "Chebyshev1", "Chebyshev2"};
Filter::Filter(FilterType type) : currentType(type), core(std::make_unique<Low12Filter>())
{
    
};

void Filter::setType(FilterType type)
{
    if(currentType != type)
    {
        currentType = type;
        switch(currentType)
        {
            case LoPass12:
            {
                core.reset(new Low12Filter());
                break;
            }
            case LoPass24:
            {
                core.reset(new ChebI());
                break;
            }
            case HiPass12:
            {
                core.reset(new Hi12Filter());
                break;
            }
            case Chebyshev1:
            {
                core.reset(new ChebI());
                break;
            }
            case Chebyshev2:
            {
                core.reset(new ChebII());
                break;
            }
                
        }
    }
}



OctaneFilter::OctaneFilter(FilterType type) :
currentType(type),
wetLevel(1.0f),
cutoff(CUTOFF_DEFAULT),
resonance(RESONANCE_DEFAULT),
sampleRate(44100.0f)
{
    
}
void OctaneFilter::setCutoff(float freq)
{
    //! check for change bc this will be called on each sample and we can save cpu by only calling \c setFilter() when something has changed
    if(freq != cutoff)
    {
        cutoff = freq;
        lFilter.setCutoff(cutoff);
        rFilter.setCutoff(cutoff);
    }
}
OctaneFilter::~OctaneFilter()
{
   
}

void OctaneFilter::setResonance(float level)
{
    if(resonance != level)
    {
        resonance = level;
        lFilter.setResonance(resonance);
        rFilter.setResonance(resonance);
    }
}
void OctaneFilter::setWetDry(float wet)
{
    wetLevel = wet;
}

void OctaneFilter::prepare(double rate, int samplesPerBlock, int numChannels)
{
    sampleRate = rate;
    lFilter.setSampleRate(rate);
    rFilter.setSampleRate(rate);
}

float OctaneFilter::processL(float input)
{
    return (wetLevel * lFilter.process(input)) + ((1.0f - wetLevel) * input);
}

float OctaneFilter::processR(float input)
{
    return (wetLevel * rFilter.process(input)) + ((1.0f - wetLevel) * input);
}
