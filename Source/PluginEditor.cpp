/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OctaneAudioProcessorEditor::OctaneAudioProcessorEditor (OctaneAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor(p),
editor(&audioProcessor.synth.paramGroup,
       &audioProcessor.tree,
       &audioProcessor.updater)
{
    addAndMakeVisible(&editor);
    audioProcessor.linkedEditor = &editor;
    setSize (1200, 900);
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
