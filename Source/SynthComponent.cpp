/*
  ==============================================================================

    SynthComponent.cpp
    Created: 19 Apr 2021 4:17:58pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "SynthComponent.h"

SynthComponent::SynthComponent(WavetableSynth* s, juce::AudioProcessorValueTreeState* t) :
pSynth(s),
tree(t),
pSlider(this, 0),
scope(pSynth->getDataToGraph(), &pSlider.mTarget),
envelope(this)
{
    pSlider.setDesc("oscPositionParam");
    pSlider.attach(tree);
    envelope.attach(tree, 0);
    
    addAndMakeVisible(pSlider);
    addAndMakeVisible(scope);
    addAndMakeVisible(envelope);
}

void SynthComponent::resized()
{
    
}

void SynthComponent::paint(juce::Graphics &g)
{
    
}
