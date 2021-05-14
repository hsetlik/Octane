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
enum FilterType
{
   LoPass12,
   LoPass24
};
class IirCalc //! note: all these functions require the vectors to already be sized correctly
{
public:
    IirCalc() : K(0.0f), norm(0.0f)
    {
    }
    void LoPass12(std::vector<float>& a, std::vector<float>& b, float cutoff, double sampleRate, float Q)
    {
        b[0] = 1.0f;
        b[1] = exp(-2.0f * juce::MathConstants<float>::pi * (cutoff / sampleRate));
        a[0] = 1.0f - b[1];
        b[1] *= -1.0f;
        a[1] = 0.0f;
        a[2] = 0.0f;
        b[2] = 0.0f;
    }
    void LoPass24(std::vector<float>& a, std::vector<float>& b, float cutoff, double sampleRate, float Q)
    {
        b[0] = 1.0f;
        K = tan(juce::MathConstants<float>::pi * cutoff / sampleRate);
        norm = 1.0f / (1.0f + K / Q + (K * K));
        a[0] = K * K * norm;
        a[1] = 2.0f * a[0];
        a[2] = a[0];
        b[1] = 2.0f * (K * K - 1.0f) * norm;
        b[2] = (1.0f - K / Q + K * K) * norm;
    }
private:
    float K, norm;
};
class OctaneFilter
{
public:
    //! make sure this enum and the string vector stay in the same order
    
    static std::vector<juce::String> FilterNames;
    //==============================================
    OctaneFilter(FilterType type = LoPass12);
    void setSampleRate(double newRate) {sampleRate = newRate;}
    float process(float input);
    void setCutoff(float freq);
    void setResonance(float level);
    void setWetDry(float wet);
    void calcVectors();
private:
    IirCalc calc;
    void setCoeffsFor(FilterType type);
    FilterType currentType;
    float wetLevel;
    float cutoff;
    float resonance;
    double sampleRate;
    std::vector<float> aCoeffs;
    std::vector<float> bCoeffs;
    stk::Iir filter;
};

