/*
  ==============================================================================

    DAHDSRGraph.h
    Created: 8 Oct 2020 10:33:41am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RgbColor.h"

class DAHDSRGraph : public juce::Component, public juce::Slider::Listener
{
public:
    DAHDSRGraph(juce::Slider* Delay,
                juce::Slider* Attack,
                juce::Slider* Hold,
                juce::Slider* Decay,
                juce::Slider* Sustain,
                juce::Slider* Release);
    ~DAHDSRGraph() {}
    void grabValues();
    void paint(juce::Graphics& g) override;
    void sliderValueChanged(juce::Slider* s) override
    {
        grabValues();
        repaint();
    }
    float exp(float linear)
    {
        auto val = 20000.0f - linear;
        return std::powf(100.0f,(-2.0f * (val / 20000.0f)));
    }
private:
    juce::Colour bgColor;
    juce::Colour traceColor;
    float fDelay, fAttack, fHold, fDecay, fSustain, fRelease;
    juce::Slider* sDelay;
    juce::Slider* sAttack;
    juce::Slider* sHold;
    juce::Slider* sDecay;
    juce::Slider* sSustain;
    juce::Slider* sRelease;
};
