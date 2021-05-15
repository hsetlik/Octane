/*
  ==============================================================================

    Filter.h
    Created: 13 May 2021 3:41:25pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#define CUTOFF_MIN 20.0f
#define CUTOFF_DEFAULT 900.0f
#define CUTOFF_MAX 10000.0f
#define CUTOFF_CENTER 600.0f
#define RESONANCE_MIN 0.1f
#define RESONANCE_MAX 10.0f
#define RESONANCE_DEFAULT 1.0f
#define CHEB_1_ORDER 8.0f
using JuceFilter = juce::dsp::IIR::Filter<float>;
using Coeffs = juce::dsp::IIR::Coefficients<float>;
enum FilterType
{
   LoPass12,
   LoPass24,
   Chebyshev1
};
class IirCalc
{ //!  this works in place ot the Coeffecient class static methods
public:
    IirCalc() : K(0.0f), norm(0.0f)
    {
    }
    void LoPass12(JuceFilter& filter, double sampleRate, float cutoff, float resonance)
    {
        
    }
    void LoPass24(JuceFilter& filter, double sampleRate, float cutoff, float resonance)
    {
        
    }
    void Chebyshev1(JuceFilter& filter, double sampleRate, float cutoff, float resonance)
    {
        
    }
private:
    float K, norm;
    std::array<float, 4> a;
    std::array<float, 4> b;
};
class OctaneFilter
{
public:
    //! make sure this enum and the string vector stay in the same order
    static std::vector<juce::String> FilterNames;
    //==============================================
    OctaneFilter(FilterType type = LoPass12);
    ~OctaneFilter();
    void setSampleRate(double newRate) {sampleRate = newRate;}
    void prepare(double rate, int samplesPerBlock, int numChannels);
    float process(float input);
    void setCutoff(float freq);
    void setResonance(float level);
    void setWetDry(float wet);
    void calcVectors();
    void setFilter();
private:
    FilterType currentType;
    float wetLevel;
    float cutoff;
    float resonance;
    double sampleRate;
    JuceFilter jFilter;
};

