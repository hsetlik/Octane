/*
  ==============================================================================

    Filter.h
    Created: 13 May 2021 3:41:25pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "/Users/hayden/Desktop/Programming/JUCEProjects/Octane/Builds/MacOSX/1.9.0/include/Iir.h"
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
using complex = std::complex<float>;
enum FilterType
{
   LoPass12,
   LoPass24,
   Chebyshev1
};

class FilterCore
{
public:
    FilterCore() :
    cutoff(CUTOFF_DEFAULT),
    resonance(RESONANCE_DEFAULT),
    sampleRate(44100.0f)
    {setup();}
    virtual ~FilterCore() {}
    void setSampleRate(double rate) {sampleRate = rate; setup();}
    virtual float process(float input) {return 0.0f;}
    virtual void setCutoff(float value) {cutoff = value; setup(); } //! override the setCutoff() and setResonance() functions only as necessary
    virtual void setResonance(float q) {resonance = q; setup(); }
    virtual void setup() {}
protected:
    float cutoff, resonance;
    double sampleRate;
};
class Low12Filter : public FilterCore
{
public:
    void setup() override {filter.setup(sampleRate, cutoff, resonance); }
    float process(float input) override {return filter.filter(input); }
private:
    Iir::RBJ::LowPass filter;
};

class ChebI : public FilterCore
{
public:
    void setup() override {filter.setup(sampleRate, cutoff, resonance); }
    float process(float input) override {return filter.filter(input); }
private:
    Iir::ChebyshevI::LowPass<DEFAULT_ORDER> filter;
};

class Filter
{
public:
    Filter(FilterType type = LoPass12);
    void setType(FilterType type);
    void prepare(double rate, int samplesPerBlock, int numChannels) {core->setSampleRate(rate); }
    void setSampleRate(double rate) {core->setSampleRate(rate); }
    void setCutoff(float c) {core->setCutoff(c);}
    void setResonance(float q) {core->setResonance(q);}
    float process(float input) {return core->process(input); }
private:
    FilterType currentType;
    std::unique_ptr<FilterCore> core;
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
    void setType(FilterType type) {sFilter.setType(type);}
    void calcVectors();
    void setFilter();
private:
    FilterType currentType;
    float wetLevel;
    float cutoff;
    float resonance;
    double sampleRate;
    Filter sFilter;
};

