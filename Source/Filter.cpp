/*
  ==============================================================================

    Filter.cpp
    Created: 13 May 2021 3:41:25pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Filter.h"
std::vector<juce::String> OctaneFilter::FilterNames{"LoPass12", "LoPass24"};

OctaneFilter::OctaneFilter(FilterType type) :
currentType(type),
wetLevel(1.0f),
cutoff(CUTOFF_DEFAULT),
resonance(RESONANCE_DEFAULT),
sampleRate(44100.0f),
aCoeffs(3, 1.0f),
bCoeffs(3, 1.0f)
{
    setCoeffsFor(currentType);
    calcVectors();
}
void OctaneFilter::setCutoff(float freq)
{
    //! check for change bc this will be called on each sample and we can save cpu by only calling \c calcVectors() when something has changed
    if(freq != cutoff)
    {
        cutoff = freq;
        calcVectors();
    }
}
void OctaneFilter::setResonance(float level)
{
    if(resonance != level)
    {
        resonance = level;
        calcVectors();
    }
}
void OctaneFilter::setWetDry(float wet)
{
    wetLevel = wet;
}
void OctaneFilter::setCoeffsFor(FilterType type)
{
    currentType = type;
    switch(currentType)
    {
        case LoPass12:
        {
            aCoeffs.resize(3, 1.0f);
            bCoeffs.resize(3, 1.0f);
            break;
        }
        case LoPass24:
        {
            aCoeffs.resize(3, 1.0f);
            bCoeffs.resize(3, 1.0f);
            break;
        }
    }
}

void OctaneFilter::calcVectors()
{
    switch(currentType)
    {
        case LoPass12:
        {
            calc.LoPass12(aCoeffs, bCoeffs, cutoff, sampleRate, resonance);
            break;
        }
        case LoPass24:
        {
            calc.LoPass24(aCoeffs, bCoeffs, cutoff, sampleRate, resonance);
            break;
        }
    }
    filter.setDenominator(aCoeffs);
    filter.setNumerator(bCoeffs);
}

float OctaneFilter::process(float input)
{
    return (wetLevel * filter.tick(input)) + ((1.0f - wetLevel) * input);
}
