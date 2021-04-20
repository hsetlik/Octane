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
envelope(this),
selector(pSynth, &scope)
{
    pSlider.setDesc("oscPositionParam");
    pSlider.attach(tree);
    envelope.attach(tree, 0);
    
    addAndMakeVisible(pSlider);
    addAndMakeVisible(scope);
    addAndMakeVisible(envelope);
    addAndMakeVisible(selector);
}

void SynthComponent::resized()
{
    //aspect is 9:8
    auto dX = getWidth() / 18;
    selector.setBounds(dX, dX, 9 * dX, dX);
    scope.setBounds(dX, 3 * dX, 9 * dX, 7 * dX);
    envelope.setBounds(11 * dX, dX, 6 * dX, 9 * dX);
    pSlider.setBounds(2 * dX, 11 * dX, 4 * dX, 4 * dX);
    
    
}

void SynthComponent::paint(juce::Graphics &g)
{
    
}
