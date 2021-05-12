/*
  ==============================================================================

    LFO.h
    Created: 4 May 2021 12:02:42pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#define LFO_POINTS 2048
#define RATE_MIN 0.0f
#define RATE_MAX 20.0f
#define RATE_DEFAULT 1.0f
#define RATE_CENTER 1.25f
using lfoArray = std::array<float, LFO_POINTS>;
using lfoFunc = std::function<lfoArray(void)>;


class LFO_Functions
{
public:
    static lfoArray createSineTable()
    {
        lfoArray dest;
        auto phaseDelta = juce::MathConstants<float>::twoPi / (float)LFO_POINTS;
        float phase = 0.0f;
        for(auto point : dest)
        {
            point = (std::sin(phase) / 2.0f) + 0.5f; //map the value between 0 and 1
            phase += phaseDelta;
        }
        return dest;
    }
    static lfoArray createSawTable()
    {
        lfoArray dest;
        float value = 1.0f;
        auto valueDelta = value / (float)LFO_POINTS;
        for(auto point : dest)
        {
            point = value;
            value -= valueDelta;
        }
        return dest;
    }
    static lfoArray createTriTable()
    {
        lfoArray dest;
        int idx = 0;
        float value = 0.0f;
        auto valueDelta = 2.0f / (float)LFO_POINTS;
        for(auto point : dest)
        {
            point = value;
            if(idx < LFO_POINTS / 2)
            {
                value += valueDelta;
            }
            else
            {
                value -= valueDelta;
            }
            ++idx;
        }
        return dest;
    }
    
};

class OctaneLFO
{
public:
    OctaneLFO();
    void resetFromFunction(lfoFunc newFunc) {points = newFunc(); }
    void resetFromArray(lfoArray& newValues) {points = newValues; }
    void setPoint(int idx, float value) {points[idx] = value; }
    void setPoint(float _phase, float value) {points[floor(_phase * LFO_POINTS)] = value; }
    void setRateHz(double hz);
    void setSampleRate(double rate) {sampleRate = rate; }
    void noteOn();
    void noteOff();
    void setTrigger(bool shouldTrigger);
    void phaseReset() {phase = 0.0f;}
    void tick();
    float getOutput();
private:
    double sampleRate;
    float phase;
    float phaseDelta;
    bool triggerMode;
    int lowerIndex;
    float lowerVal;
    float upperVal;
    float skew;
    float lastOutput;
    lfoArray points;
};


