/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OctaneAudioProcessorEditor::OctaneAudioProcessorEditor (OctaneAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor(p), editor(&audioProcessor.synth.paramGroup)
{
    addAndMakeVisible(&editor);
    setSize (1000, 800);
}

OctaneAudioProcessorEditor::~OctaneAudioProcessorEditor()
{
}

//==============================================================================
void OctaneAudioProcessorEditor::paint (juce::Graphics& g)
{
    
}

void OctaneAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    editor.setBounds(getBounds());
}
