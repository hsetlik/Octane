/*
  ==============================================================================

    OctaneEditor.cpp
    Created: 1 May 2021 6:02:07pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "OctaneEditor.h"

EnvelopePanel::EnvelopeGraph::EnvelopeGraph(EnvelopePanel* panel) :
linkedPanel(panel),
cMin(pow(juce::MathConstants<float>::euler, 0.0f)),
cMax(pow(juce::MathConstants<float>::euler, 1.0f))
{
    startTimerHz(REPAINT_FPS);
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
    updateNumbers();
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
                             SynthParam* release,
                             SynthParam* src,
                             apvts* tree) :
delayComp(delay),
attackComp(attack),
holdComp(hold),
decayComp(decay),
sustainComp(sustain),
releaseComp(release),
srcComp(src),
graph(this),
linkedTree(tree)
{
    addAndMakeVisible(&delayComp);
    addAndMakeVisible(&attackComp);
    addAndMakeVisible(&holdComp);
    addAndMakeVisible(&decayComp);
    addAndMakeVisible(&sustainComp);
    addAndMakeVisible(&releaseComp);
    addAndMakeVisible(&graph);
    addAndMakeVisible(&srcComp);
    
    delayAttach.reset(new apvts::SliderAttachment(*linkedTree, delayComp.linkedParam->name, delayComp.mainSlider));
    attackAttach.reset(new apvts::SliderAttachment(*linkedTree, attackComp.linkedParam->name, attackComp.mainSlider));
    holdAttach.reset(new apvts::SliderAttachment(*linkedTree, holdComp.linkedParam->name, holdComp.mainSlider));
    decayAttach.reset(new apvts::SliderAttachment(*linkedTree, decayComp.linkedParam->name, decayComp.mainSlider));
    sustainAttach.reset(new apvts::SliderAttachment(*linkedTree, sustainComp.linkedParam->name, sustainComp.mainSlider));
    releaseAttach.reset(new apvts::SliderAttachment(*linkedTree, releaseComp.linkedParam->name, releaseComp.mainSlider));
}

void EnvelopePanel::resized()
{
    auto fBounds = getBounds().toFloat();
    auto dX = fBounds.getWidth() / 6.0f;
    auto halfHeight = fBounds.getHeight() / 2.0f;
    auto gBounds = juce::Rectangle<int>(0, 0, 6 * dX, halfHeight).reduced(dX / 3);
    graph.setBounds(gBounds);
    
    
    delayComp.setBounds(0, halfHeight, dX, halfHeight);
    attackComp.setBounds(dX, halfHeight, dX, halfHeight);
    holdComp.setBounds(2 * dX, halfHeight, dX, halfHeight);
    decayComp.setBounds(3 * dX, halfHeight, dX, halfHeight);
    sustainComp.setBounds(4 * dX, halfHeight, dX, halfHeight);
    releaseComp.setBounds(5 * dX, halfHeight, dX, halfHeight);
    
    dX = getWidth() / 24.0f;
    srcComp.setBounds(18 * dX, 2 * dX, 4 * dX, 4 * dX);
    srcComp.toFront(true);
}

void EnvelopePanel::paint(juce::Graphics &g)
{
    
}
//=========================================================================
LFOPanel::RetrigButton::RetrigButton() : juce::ShapeButton("retrigButton",  juce::Colours::black, juce::Colours::black, juce::Colours::black)
{
    setClickingTogglesState(true);
}

void LFOPanel::RetrigButton::paintButton(juce::Graphics &g, bool mouseOver, bool isMouseDown)
{
    auto fBounds = getLocalBounds().toFloat();
    auto corner = fBounds.getWidth() / 10.0f;
    if(getToggleState())
        g.setColour(UXPalette::darkGray);
    else
        g.setColour(UXPalette::lightGray);
    g.fillRoundedRectangle(fBounds, corner);
}



//=========================================================================

LFOPanel::LFOPanel(SynthParam* rate, SynthParam* retrig, SynthParam* src, lfoArray* array, apvts* tree, OctaneUpdater* updater, int index) :
rateComp(rate),
outputComp(src),
editor(array, updater, index),
meter(src),
retrigParam(retrig),
linkedArray(array),
linkedTree(tree),
lfoIndex(index)
{
    addAndMakeVisible(&rButton);
    rButton.addListener(this);
    
    addAndMakeVisible(&rateComp);
    addAndMakeVisible(&outputComp);
    addAndMakeVisible(&meter);
    addAndMakeVisible(&editor);
    
    rateAttach.reset(new apvts::SliderAttachment(*linkedTree, rateComp.linkedParam->name, rateComp.mainSlider));
    
}

void LFOPanel::resized()
{
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getWidth() / 6.0f;
    auto dY = fBounds.getHeight() / 6.0f;
    auto retrigBounds = juce::Rectangle<float>(4 * dX, dY, dX, dY);
    rButton.setBounds(retrigBounds.reduced(dX / 3.0f).toType<int>());
    auto squareSide = (dX > dY) ? dY : dX;
    auto rateBounds = juce::Rectangle<int>(0, 0, 2 * squareSide, 2 * squareSide);
    auto sourceBounds = juce::Rectangle<int>(2 * squareSide, squareSide / 3, squareSide, squareSide);
    auto editorBounds = juce::Rectangle<int>(dX / 2, 2 * squareSide, 3.5 * squareSide, 3 * squareSide);
    auto meterBounds = juce::Rectangle<int>(5 * dX, 2 * dY, dX / 2, 3 * dY);
    rateComp.setBounds(rateBounds.reduced((int)dX / 3));
    outputComp.setBounds(sourceBounds);
    meter.setBounds(meterBounds);
    editor.setBounds(editorBounds);
}

void LFOPanel::paint(juce::Graphics &g)
{
    
}
//==============================================================================
OscillatorPanel::OscillatorPanel(SynthParam* lParam, SynthParam* pParam, std::vector<std::vector<float>> graphData, apvts* tree) :
levelComp(lParam),
posComp(pParam),
display(std::make_unique<WaveGraphOpenGL>(graphData, pParam)),
linkedTree(tree)
{
    addAndMakeVisible(&levelComp);
    addAndMakeVisible(&posComp);
    addAndMakeVisible(*display);
    levelAttach.reset(new apvts::SliderAttachment(*linkedTree, levelComp.linkedParam->name, levelComp.mainSlider));
    posAttach.reset(new apvts::SliderAttachment(*linkedTree, posComp.linkedParam->name, posComp.mainSlider));
}

void OscillatorPanel::resized()
{
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getWidth() / 10;
    auto dY = fBounds.getHeight() / 10;
    auto squareSide = (dX > dY) ? dY : dX;
    auto trim = dX / 2.5f;
    auto displayBounds = juce::Rectangle<float>(0.0f, dY, 5 * dX, 8 * dY);
    display->setBounds(displayBounds.reduced(trim).toType<int>());
    auto topBounds = juce::Rectangle<float>(5 * dX, dY, 5 * squareSide, 5 * squareSide);
    auto bottomBounds = juce::Rectangle<float>(5 * dX, 5 * dY, 5 * squareSide, 5 * squareSide);
    posComp.setBounds(topBounds.reduced(trim).toType<int>());
    levelComp.setBounds(bottomBounds.reduced(trim).toType<int>());
}

void OscillatorPanel::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::white);
}
//===============================================================================
SoundSourcePanel::SoundSourcePanel(SynthParameterGroup* allParams, int index, apvts* tree) :
oscPanel(allParams->oscLevels[index],
         allParams->oscPositions[index],
         allParams->oscGraphVectors[index],
         tree),
ampEnvPanel(allParams->aDelays[index],
         allParams->aAttacks[index],
         allParams->aHolds[index],
         allParams->aDecays[index],
         allParams->aSustains[index],
         allParams->aReleases[index],
         allParams->oscAmpEnvs[index],
            tree),
modEnvPanel(allParams->mDelays[index],
         allParams->mAttacks[index],
         allParams->mHolds[index],
         allParams->mDecays[index],
         allParams->mSustains[index],
         allParams->mReleases[index],
         allParams->oscModEnvs[index],
            tree),
linkedTree(tree)
{
    addAndMakeVisible(&oscPanel);
    addAndMakeVisible(&ampEnvPanel);
    addAndMakeVisible(&modEnvPanel);
}

void SoundSourcePanel::resized()
{
    auto fBounds = getLocalBounds().toFloat();
    oscPanel.setBounds(0, 0, fBounds.getWidth(), fBounds.getHeight() / 2);
    auto ampBounds = juce::Rectangle<int>(0, fBounds.getHeight() / 2, fBounds.getWidth() / 2, fBounds.getHeight() / 2);
    ampEnvPanel.setBounds(ampBounds);
    auto modBounds = juce::Rectangle<int>(fBounds.getWidth() / 2, fBounds.getHeight() / 2, fBounds.getWidth() / 2, fBounds.getHeight() / 2);
    modEnvPanel.setBounds(modBounds);
}
//==============================================================================

OctaneEditor::OctaneEditor(SynthParameterGroup* pGroup, apvts* tree, OctaneUpdater* update) :
paramGroup(pGroup),
linkedTree(tree),
linkedUpdater(update)
{
    for(int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        oscPanels.add(new SoundSourcePanel(paramGroup, i, tree));
        auto panel = oscPanels.getLast();
        addAndMakeVisible(panel);
    }
    for(int i = 0; i < NUM_LFOS; ++i)
    {
        auto pRate = paramGroup->lfoRates[i];
        auto pRetrig = paramGroup->lfoRetriggers[i];
        auto pOut = paramGroup->lfoOutputs[i];
        auto pArray = &paramGroup->lfoShapes[i];
        lfoPanels.add(new LFOPanel(pRate, pRetrig, pOut, pArray, tree, linkedUpdater, i));
        auto panel = lfoPanels.getLast();
        addAndMakeVisible(panel);
    }
}

void OctaneEditor::resized()
{
    auto iBounds = getLocalBounds();
    auto dX = iBounds.getWidth() / 5;
    auto halfX = (4 * dX) / 2;
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
    auto lHeight = iBounds.getHeight() / NUM_LFOS;
    for(idx = 0; idx < NUM_LFOS; ++idx)
    {
        lfoPanels[idx]->setBounds(4 * dX, lHeight * idx, dX, lHeight);
    }
}

void OctaneEditor::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::white);
}
