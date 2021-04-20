/*
  ==============================================================================

    SynthComponent.h
    Created: 19 Apr 2021 4:17:58pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "EnvelopeComponent.h"
#include "WavetableScope.h"
#include "Synthesizer.h"
#include "SliderTypes.h"

class SynthComponent : public juce::Component, public juce::DragAndDropContainer, public juce::ComboBox::Listener
{
public:
    SynthComponent(WavetableSynth* s, juce::AudioProcessorValueTreeState* t);
    void comboBoxChanged(juce::ComboBox* box) override;
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    WavetableSynth* pSynth;
    juce::AudioProcessorValueTreeState* tree;
    TablePositionSlider pSlider;
    WaveScopeHolder scope;
    DAHDSRPanel envelope;
    TableSelector selector;
};
