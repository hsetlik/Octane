/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class OctaneAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    OctaneAudioProcessorEditor (OctaneAudioProcessor&);
    ~OctaneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    OctaneAudioProcessor& audioProcessor;
    MainEditor editor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OctaneAudioProcessorEditor)
};
