/*
  ==============================================================================

    Filter.cpp
    Created: 13 May 2021 3:41:25pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Filter.h"
std::vector<juce::String> OctaneFilter::FilterNames{"LoPass12", "LoPass24", "Chebyshev1"};
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
            case Chebyshev1:
            {
                core.reset(new ChebI());
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
    setFilter();
}
void OctaneFilter::setCutoff(float freq)
{
    //! check for change bc this will be called on each sample and we can save cpu by only calling \c setFilter() when something has changed
    if(freq != cutoff)
    {
        cutoff = freq;
        setFilter();
    }
}
OctaneFilter::~OctaneFilter()
{
    printf("NEW FILTER\n");
    for(auto val : jFilter.coefficients->coefficients)
        printf("Value: %f\n", val);
}

void OctaneFilter::setResonance(float level)
{
    if(resonance != level)
    {
        resonance = level;
        setFilter();
    }
}
void OctaneFilter::setWetDry(float wet)
{
    wetLevel = wet;
}

void OctaneFilter::prepare(double rate, int samplesPerBlock, int numChannels)
{
    sampleRate = rate;
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = numChannels;
    jFilter.prepare(spec);
    jFilter.reset();
}

float OctaneFilter::process(float input)
{
    return (wetLevel * jFilter.processSample(input)) + ((1.0f - wetLevel) * input);
}

void OctaneFilter::setFilter()
{
    switch(currentType)
    {
        case LoPass12:
        {
            jFilter.coefficients = Coeffs::makeLowPass(sampleRate, cutoff, resonance);
            break;
        }
        case LoPass24:
        {
            jFilter.coefficients = Coeffs::makeLowPass(sampleRate, cutoff, resonance);
            break;
        }
        case Chebyshev1:
        {
            jFilter.coefficients = Coeffs::makeLowPass(sampleRate, cutoff, resonance);
            break;
        }
    }
}
