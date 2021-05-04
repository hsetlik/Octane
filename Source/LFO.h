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
using lfoArray = std::array<float, LFO_POINTS>;
using lfoFunc = std::function<void(lfoArray&)>;


class LFO_Functions
{
public:
    static void createSineTable(lfoArray& dest)
    {
        auto phaseDelta = juce::MathConstants<float>::twoPi / (float)LFO_POINTS;
        float phase = 0.0f;
        for(auto point : dest)
        {
            point = (std::sin(phase) / 2.0f) + 0.5f; //map the value between 0 and 1
            phase += phaseDelta;
        }
    }
    static void createSawTable(lfoArray& dest)
    {
        float value = 1.0f;
        auto valueDelta = value / (float)LFO_POINTS;
        for(auto point : dest)
        {
            point = value;
            value -= valueDelta;
        }
    }
    static void createTriTable(lfoArray& dest)
    {
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
    }
    
};

class OctaneLFO
{
public:
    //! LFOs are iniitalized with a periodic function with an input in the range 0-1 (sine, saw, triangle, etc) and then shaped later
    OctaneLFO(lfoFunc srcFunction);
    void resetFromFunction(lfoFunc newFunc) {newFunc(points); }
    void setPoint(int idx, float value) {points[idx] = value; }
    void setPoint(float _phase, float value) {points[floor(_phase * LFO_POINTS)] = value; }
    void setPlaying(bool shouldPlay) {isPlaying = shouldPlay; }
    void setRateHz(double hz);
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
    bool isPlaying;
    bool triggerMode;
    int lowerIndex;
    int lowerVal;
    int upperVal;
    float skew;
    lfoArray points;
};


