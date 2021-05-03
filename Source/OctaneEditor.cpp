/*
  ==============================================================================

    OctaneEditor.cpp
    Created: 1 May 2021 6:02:07pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "OctaneEditor.h"

EnvelopePanel::EnvelopeGraph::EnvelopeGraph(EnvelopePanel* panel) : linkedPanel(panel)
{
    setFramesPerSecond(REPAINT_FPS);
    
}

void EnvelopePanel::EnvelopeGraph::updateNumbers()
{
    lengthMs = 0.0f;
    lengthMs += linkedPanel->delayComp.linkedParam->getBaseValue();
    delayEnd = lengthMs;
    lengthMs += linkedPanel->attackComp.linkedParam->getBaseValue();
    attackEnd = lengthMs;
    lengthMs += linkedPanel->holdComp.linkedParam->getBaseValue();
    holdEnd  = lengthMs;
    lengthMs += linkedPanel->decayComp.linkedParam->getBaseValue();
    decayEnd = lengthMs;
    lengthMs += 200.0f; //represent the sustain phase length as 200 ms
    sustainEnd = lengthMs;
    lengthMs += linkedPanel->releaseComp.linkedParam->getBaseValue();
    releaseEnd = lengthMs;
    //remap all those values to the range 0-1
    delayEnd /= lengthMs;
    attackEnd /= lengthMs;
    holdEnd /= lengthMs;
    decayEnd /= lengthMs;
    sustainEnd /= lengthMs;
    releaseEnd /= lengthMs;
    //get the sustain as well
    fSustain = linkedPanel->sustainComp.linkedParam->getBaseValue();
}

void EnvelopePanel::EnvelopeGraph::paint(juce::Graphics &g)
{
    g.fillAll(UXPalette::darkBkgnd);
    g.setColour(UXPalette::highlight);
    auto fBounds = getLocalBounds().toFloat();
    auto fullWidth = fBounds.getWidth();
    juce::Path path;
    path.startNewSubPath(0.0f, fBounds.getHeight());
    //from start to end of delay
    path.lineTo(fullWidth * delayEnd, fBounds.getHeight());
    //from end of delay to max attack
    path.lineTo(fullWidth * attackEnd, fBounds.getHeight() * 0.2f);
    //to end of hold
    path.lineTo(fullWidth * holdEnd, fBounds.getHeight() * 0.2f);
    //to end of decay
    path.lineTo(fullWidth * decayEnd, fBounds.getHeight() - (fBounds.getHeight() * fSustain));
    //to end of sustain
    path.lineTo(fullWidth * sustainEnd, fBounds.getHeight() - (fBounds.getHeight() * fSustain));
    //to bottom right corner
    path.lineTo(fullWidth, fBounds.getHeight());
    
    auto stroke = juce::PathStrokeType(1.0f);
    
    g.strokePath(path, stroke);
}

//==================================================
EnvelopePanel::EnvelopePanel(SynthParam* delay,
                             SynthParam* attack,
                             SynthParam* hold,
                             SynthParam* decay,
                             SynthParam* sustain,
                             SynthParam* release) :
delayComp(delay),
attackComp(attack),
holdComp(hold),
decayComp(decay),
sustainComp(sustain),
releaseComp(release)
{
    addAndMakeVisible(&delayComp);
    addAndMakeVisible(&attackComp);
    addAndMakeVisible(&holdComp);
    addAndMakeVisible(&decayComp);
    addAndMakeVisible(&sustainComp);
    addAndMakeVisible(&releaseComp);
}

void EnvelopePanel::resized()
{
    
}

void EnvelopePanel::paint(juce::Graphics &g)
{
    
}

//==============================================================================
OscillatorPanel::OscillatorPanel(SynthParam* lParam, SynthParam* pParam) :
levelComp(lParam),
posComp(pParam)
{
    addAndMakeVisible(&levelComp);
    addAndMakeVisible(&posComp);
}

void OscillatorPanel::resized()
{
    auto dX = getWidth() / 2;
    auto dY = getHeight() / 2;
    auto topY = dY / 2;
    auto leftX = (dX / 2) - (dY / 2);
    levelComp.setBounds(leftX, topY, dY, dY);
    posComp.setBounds(dX + leftX, topY, dY, dY);
}

void OscillatorPanel::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::white);
}

//==============================================================================

OctaneEditor::OctaneEditor(SynthParameterGroup* pGroup) : paramGroup(pGroup)
{
    glContext.attachTo(*this);
    for(int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        auto lPtr = paramGroup->oscLevels[i];
        auto pPtr = paramGroup->oscPositions[i];
        oscPanels.add(new OscillatorPanel(lPtr, pPtr));
        auto panel = oscPanels.getLast();
        addAndMakeVisible(panel);
    }
}

void OctaneEditor::resized()
{
    auto halfX = getLocalBounds().getWidth() / 2;
    auto halfY = getLocalBounds().getHeight() / 2;
    oscBoundRects.clear();
    oscBoundRects.push_back(juce::Rectangle<int>(0, 0, halfX, halfY));
    oscBoundRects.push_back(juce::Rectangle<int>(halfX, 0, halfX, halfY));
    oscBoundRects.push_back(juce::Rectangle<int>(0, halfY, halfX, halfY));
    oscBoundRects.push_back(juce::Rectangle<int>(halfX, halfY, halfX, halfY));
    int idx = 0;
    for(auto osc : oscPanels)
    {
        osc->setBounds(oscBoundRects[idx]);
        ++idx;
    }
}
