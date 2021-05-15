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
#define DEFAULT_ORDER 4
using JuceFilter = juce::dsp::IIR::Filter<float>;
using Coeffs = juce::dsp::IIR::Coefficients<float>;
using OrderArr = std::array<float, DEFAULT_ORDER>;
enum FilterType
{
   LoPass12,
   LoPass24,
   Chebyshev1
};
struct FilterArray
{ //! stores the previous input/output samples of the filter
public:
    FilterArray()
    {
        std::fill(data.begin(), data.end(), 0.0f);
    }
    float& operator [](int index)
    {
        return data[index];
    }
    void pushFront(float value)
    {
        for(index = DEFAULT_ORDER - 1; index > 0; --index)
        {
            data[index] = data[index - 1];
        }
        data[0] = value;
    }
private:
    int index;
    OrderArr data;
};
class FilterCore
{ //! the processing guts of an IIR filter. just holds arrays for coefficients and past i/o and calculates the difference equation for each sample passed in
public:
    FilterCore(FilterType type=LoPass12) : currentType(type), sampleRate(44100.0f)
    {
    }
    void setSampleRate(double rate) {sampleRate = rate; }
    float process(float input);
    OrderArr aCoeffs;
    OrderArr bCoeffs;
private:
    FilterType currentType;
    double sampleRate;
    FilterArray inputs;
    FilterArray outputs;
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

