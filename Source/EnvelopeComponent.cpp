/*
  ==============================================================================

    EnvelopeComponent.cpp
    Created: 7 Mar 2021 8:12:47pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "EnvelopeComponent.h"

DAHDSRPanel::DAHDSRPanel(juce::DragAndDropContainer* c) :
sDelay(c, DelaySlider, 0),
sAttack(c, AttackSlider, 0),
sHold(c, HoldSlider, 0),
sDecay(c, DecaySlider, 0),
sSustain(c, SustainSlider, 0),
sRelease(c, ReleaseSlider, 5),
lDelay(&sDelay.mTarget, "ms"),
lAttack(&sAttack.mTarget, "ms"),
lHold(&sHold.mTarget, "ms"),
lDecay(&sDecay.mTarget, "ms"),
lSustain(&sSustain.mTarget, ""),
lRelease(&sRelease.mTarget, "ms"),
graph(&sDelay.mTarget, &sAttack.mTarget, &sHold.mTarget, &sDecay.mTarget, &sSustain.mTarget, &sRelease.mTarget),
envModSource(c, "dummySource")
{
    addAndMakeVisible(&sDelay);
    addAndMakeVisible(&sAttack);
    addAndMakeVisible(&sHold);
    addAndMakeVisible(&sDecay);
    addAndMakeVisible(&sSustain);
    addAndMakeVisible(&sRelease);
    
    addAndMakeVisible(&lDelay);
    addAndMakeVisible(&lAttack);
    addAndMakeVisible(&lHold);
    addAndMakeVisible(&lDecay);
    addAndMakeVisible(&lSustain);
    addAndMakeVisible(&lRelease);
    
    addAndMakeVisible(&graph);
    addAndMakeVisible(&envModSource);
}

void DAHDSRPanel::resized()
{
    auto dX = getWidth() / 12;
    auto dY = getHeight() / 8;
    graph.setBounds(getWidth() / 40, getWidth() / 40, 4 * dX, 3 * dY);
    
    sDelay.setBounds(0, 3 * dY, 2 * dX, 2 * dX);
    sAttack.setBounds(2 * dX, 3 * dY, 2 * dX, 2 * dX);
    sHold.setBounds(4 * dX, 3 * dY, 2 * dX, 2 * dX);
    sDecay.setBounds(6 * dX, 3 * dY, 2 * dX, 2 * dX);
    sSustain.setBounds(8 * dX, 3 * dY, 2 * dX, 2 * dX);
    sRelease.setBounds(10 * dX, 3 * dY, 2 * dX, 2 * dX);
    
    lDelay.setBounds(0, 5 * dY, 2 * dX, dY);
    lAttack.setBounds(2 * dX, 5 * dY, 2 * dX, dY);
    lHold.setBounds(4 * dX, 5 * dY, 2 * dX, dY);
    lDecay.setBounds(6 * dX, 5 * dY, 2 * dX, dY);
    lSustain.setBounds(8 * dX, 5 * dY, 2 * dX, dY);
    lRelease.setBounds(10 * dX, 5 * dY, 2 * dX, dY);
    
    envModSource.setBounds(7 * dX, dY, dX, dX);
}

void DAHDSRPanel::paint(juce::Graphics &g)
{
    g.setColour(envModSource.getColor().darker(0.3f));
    g.fillRect(getLocalBounds());
    g.setColour(envModSource.getColor().darker(0.1f));
    g.fillRect(getLocalBounds().reduced(getLocalBounds().getWidth() / 45));
}

void DAHDSRPanel::attach(juce::AudioProcessorValueTreeState *tree, int suffix)
{
    auto iStr = juce::String(suffix);
    aDelay.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(*tree, "delayParam" + iStr, sDelay.mTarget));
    aAttack.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(*tree, "attackParam" + iStr, sAttack.mTarget));
    aHold.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(*tree, "holdParam" + iStr, sHold.mTarget));
    aDecay.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(*tree, "decayParam" + iStr, sDecay.mTarget));
    aSustain.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(*tree, "sustainParam" + iStr, sSustain.mTarget));
    aRelease.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(*tree, "releaseParam" + iStr, sRelease.mTarget));
}
