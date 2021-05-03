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
releaseComp(release),
graph(this)
{
    addAndMakeVisible(&delayComp);
    addAndMakeVisible(&attackComp);
    addAndMakeVisible(&holdComp);
    addAndMakeVisible(&decayComp);
    addAndMakeVisible(&sustainComp);
    addAndMakeVisible(&releaseComp);
    addAndMakeVisible(&graph);
}

void EnvelopePanel::resized()
{
    auto fBounds = getBounds().toFloat();
    auto dX = fBounds.getWidth() / 6.0f;
    auto halfHeight = fBounds.getHeight() / 2.0f;
    graph.setBounds(0, 0, 6 * dX, halfHeight);
    
    delayComp.setBounds(0, halfHeight, dX, halfHeight);
    attackComp.setBounds(dX, halfHeight, dX, halfHeight);
    holdComp.setBounds(2 * dX, halfHeight, dX, halfHeight);
    decayComp.setBounds(3 * dX, halfHeight, dX, halfHeight);
    sustainComp.setBounds(4 * dX, halfHeight, dX, halfHeight);
    releaseComp.setBounds(5 * dX, halfHeight, dX, halfHeight);
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
//===============================================================================
SoundSourcePanel::SoundSourcePanel(SynthParameterGroup* allParams, int index) :
oscPanel(allParams->oscLevels[index],
         allParams->oscPositions[index]),
envPanel(allParams->aDelays[index],
         allParams->aAttacks[index],
         allParams->aHolds[index],
         allParams->aDecays[index],
         allParams->aSustains[index],
         allParams->aReleases[index])
{
    addAndMakeVisible(&oscPanel);
    addAndMakeVisible(&envPanel);
}

void SoundSourcePanel::resized()
{
    auto fBounds = getLocalBounds().toFloat();
    oscPanel.setBounds(0, 0, fBounds.getWidth(), fBounds.getHeight() / 2);
    envPanel.setBounds(0, fBounds.getHeight() / 2, fBounds.getWidth() / 2, fBounds.getHeight() / 2);
}
//==============================================================================

OctaneEditor::OctaneEditor(SynthParameterGroup* pGroup) : paramGroup(pGroup)
{
    glContext.attachTo(*this);
    for(int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        oscPanels.add(new SoundSourcePanel(paramGroup, i));
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
