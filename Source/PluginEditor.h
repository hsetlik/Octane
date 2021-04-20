/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SynthComponent.h"

//==============================================================================
/**
*/
class OctaneAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    OctaneAudioProcessorEditor (OctaneAudioProcessor&);
    ~OctaneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SynthComponent synthPanel;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OctaneAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OctaneAudioProcessorEditor)
};
