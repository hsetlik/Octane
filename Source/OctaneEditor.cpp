/*
  ==============================================================================

    OctaneEditor.cpp
    Created: 1 May 2021 6:02:07pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "OctaneEditor.h"

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
    levelComp.setBounds(0, 0, dY, dY);
    posComp.setBounds(dX, 0, dY, dY);
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
