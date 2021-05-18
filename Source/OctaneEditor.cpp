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
    auto squareSide = (dX > dY) ? dY : dX;
    auto rateBounds = juce::Rectangle<int>(0, 0, 3 * squareSide, 3 * squareSide);
    auto sourceBounds = juce::Rectangle<int>(3 * squareSide, squareSide / 2, squareSide, squareSide);
    auto editorBounds = juce::Rectangle<int>(dX / 2, 3 * squareSide, 3.5f * squareSide, 3 * squareSide);
    auto meterBounds = juce::Rectangle<int>(5 * dX, 2 * dY, dX / 2, 3 * dY);
    auto retrigBounds = juce::Rectangle<int>(5 * dX, dY / 2, dX / 2, dY / 2);
    rateComp.setBounds(rateBounds.reduced(getWidth() / 15));
    outputComp.setBounds(sourceBounds);
    meter.setBounds(meterBounds);
    editor.setBounds(editorBounds);
    rButton.setBounds(retrigBounds);
}

void LFOPanel::paint(juce::Graphics &g)
{
    
}
//====================================================================================================
FilterPanel::FilterPanel(SynthParam* cutoff, SynthParam* resonance, SynthParam* wetDry, OctaneUpdater* updater, apvts* tree) :
cutoffParam(cutoff),
resonanceParam(resonance),
linkedUpdater(updater),
linkedTree(tree),
cutoffComp(cutoff),
resonanceComp(resonance),
wetDryComp(wetDry)
{
    addAndMakeVisible(&typeBox);
    addAndMakeVisible(&cutoffComp);
    addAndMakeVisible(&resonanceComp);
    addAndMakeVisible(&wetDryComp);
    typeBox.addListener(this);
    int startIndex = 1;
    for(auto fType : OctaneFilter::FilterNames)
    {
        typeBox.addItem(fType, startIndex);
        ++startIndex;
    }
    typeBox.setSelectedId(1);
}

void FilterPanel::comboBoxChanged(juce::ComboBox *b)
{
    auto idx = b->getSelectedItemIndex();
    linkedUpdater->linkedSynth->setFilterType(idx);
}

void FilterPanel::resized()
{
    auto iBounds = getLocalBounds();
    auto bBounds = iBounds.removeFromTop(getHeight() / 8);
    typeBox.setBounds(bBounds.getX(), bBounds.getHeight() / 4, bBounds.getWidth() * 0.75f, bBounds.getHeight() * 0.75f);
    auto dX = iBounds.getWidth() / 3;
    auto dY = iBounds.getHeight();
    auto squareSide = (dX > dY) ? dY : dX;
    auto cushion = squareSide / 6;
    auto cBounds = juce::Rectangle<int>(iBounds.getX(), iBounds.getY(), squareSide, squareSide);
    auto rBounds = juce::Rectangle<int>(iBounds.getX() + dX, iBounds.getY(), squareSide, squareSide);
    auto wBounds = juce::Rectangle<int>(iBounds.getX() + (2 * dX), iBounds.getY(), squareSide, squareSide);
    cutoffComp.setBounds(cBounds.reduced(cushion));
    resonanceComp.setBounds(rBounds.reduced(cushion));
    wetDryComp.setBounds(wBounds.reduced(cushion));
}

void FilterPanel::paint(juce::Graphics &g)
{
    
}
//====================================================================================================
WaveSelector::WaveSelector(SynthParam* pParam, OctaneUpdater* updater, int index) :
positionParam(pParam),
linkedUpdater(updater),
oscIndex(index),
selectedFile(updater->linkedSynth->fileForOsc(index)),
display(std::make_unique<WaveGraphOpenGL>(updater->linkedSynth->paramGroup.oscGraphVectors[index], pParam)),
waveFiles(&updater->linkedSynth->waveFiles),
needsToUpdate(false),
textButton(selectedFile.getFileName()),
selectedIndex(waveFiles->indexOf(selectedFile))
{
    selectedFile = linkedUpdater->linkedSynth->fileForOsc(index);
    addAndMakeVisible(&*display);
    addAndMakeVisible(&textButton);
    addAndMakeVisible(&lastButton);
    addAndMakeVisible(&nextButton);
    lastButton.addListener(this);
    nextButton.addListener(this);
    textButton.setButtonText(selectedFile.getFileName());
    printf("Button says: %s\n", textButton.getButtonText().toRawUTF8());
    textButton.onClick = [&]
    {
        juce::PopupMenu menu;
        for(int i = 0; i < waveFiles->size(); ++i)
        {
            menu.addItem(waveFiles->getUnchecked(i).getFileName(), [this, i](){this->replaceFromIndex(i);});
            if(i % 10 == 0)
                menu.addColumnBreak();
        }
        menu.showMenuAsync(juce::PopupMenu::Options{}.withTargetComponent(&textButton));
    };
}
void WaveSelector::buttonClicked(juce::Button *b)
{
    if(b == &lastButton)
    {
        if(selectedIndex > 0)
            replaceFromIndex(selectedIndex - 1);
        else
            replaceFromIndex(waveFiles->size() - 1);
    }
    if(b == &nextButton)
    {
        if(selectedIndex < waveFiles->size() - 1)
            replaceFromIndex(selectedIndex + 1);
        else
            replaceFromIndex(0);
    }
}
void WaveSelector::replaceFromIndex(int index)
{
    selectedIndex = index;
    auto file = waveFiles->getUnchecked(index);
    textButton.setButtonText(file.getFileName());
    linkedUpdater->stageWaveChange(oscIndex, file);
    needsToUpdate = true;
    triggerAsyncUpdate();
}
void WaveSelector::handleAsyncUpdate()
{
    if(needsToUpdate)
    {
        linkedUpdater->linkedSynth->updateGraphData();
        auto vec = linkedUpdater->linkedSynth->paramGroup.oscGraphVectors[oscIndex];
        display.reset(new WaveGraphOpenGL(vec, positionParam));
        addAndMakeVisible(&*display);
        resized();
        needsToUpdate = false;
    }
}
void WaveSelector::resized()
{
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getHeight() / 9.0f;
    auto buttonBounds = fBounds.removeFromTop(dX);
    textButton.setBounds(buttonBounds.toType<int>());
    textButton.changeWidthToFitText();
    display->setBounds(fBounds.toType<int>());
    juce::Rectangle<int> otherButtonBounds;
    if(textButton.getWidth() <= (int)fBounds.getWidth() * 0.7f)
    {
        otherButtonBounds = juce::Rectangle<int>((int)fBounds.getWidth() * 0.7f,
                                                 0,
                                                 (int)fBounds.getWidth() * 0.3f,
                                                 (int)buttonBounds.getHeight());
        
    }
    else
    {
        otherButtonBounds = juce::Rectangle<int>(textButton.getWidth(),
                                                 0,
                                                 (int)fBounds.getWidth() - textButton.getWidth(),
                                                 (int)buttonBounds.getHeight());
    }
    nextButton.setBounds(otherButtonBounds.removeFromRight(otherButtonBounds.getWidth() / 2).reduced(buttonBounds.getHeight() / 8));
    lastButton.setBounds(otherButtonBounds.reduced(buttonBounds.getHeight() / 8));
}
//==============================================================================
OscillatorPanel::OscillatorPanel(SynthParam* lParam, SynthParam* pParam, SynthParam* panParam, SynthParam* powerParam, OctaneUpdater* updater, apvts* tree, int index) :
levelComp(lParam),
posComp(pParam),
panComp(panParam),
powerComp(powerParam),
display(pParam, updater, index),
linkedTree(tree)
{
    addAndMakeVisible(&levelComp);
    addAndMakeVisible(&posComp);
    addAndMakeVisible(&panComp);
    addAndMakeVisible(&powerComp);
    addAndMakeVisible(&display);
    levelAttach.reset(new apvts::SliderAttachment(*linkedTree, levelComp.linkedParam->name, levelComp.mainSlider));
    posAttach.reset(new apvts::SliderAttachment(*linkedTree, posComp.linkedParam->name, posComp.mainSlider));
    panAttach.reset(new apvts::SliderAttachment(*linkedTree, panComp.linkedParam->name, panComp.mainSlider));
}

void OscillatorPanel::resized()
{
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getWidth() / 10;
    auto dY = fBounds.getHeight() / 10;
    auto squareSide = (dX > dY) ? dY : dX;
    auto trim = dX / 2.5f;
    auto displayBounds = juce::Rectangle<float>(0.0f, dY, 5 * dX, 8 * dY);
    display.setBounds(displayBounds.reduced(trim).toType<int>());
    auto topBounds = juce::Rectangle<float>(5 * dX, dY, 5 * squareSide, 5 * squareSide);
    auto topRightBounds = juce::Rectangle<float>(5 * dX + 5 * squareSide, dY, 5 * squareSide, 5 * squareSide);
    auto bottomBounds = juce::Rectangle<float>(5 * dX, 5 * dY, 5 * squareSide, 5 * squareSide);
    auto bottomRightBounds = juce::Rectangle<float>(5 * dX + 5 * squareSide, 5 * dY, 5 * squareSide, 5 * squareSide);
    posComp.setBounds(topBounds.reduced(trim).toType<int>());
    levelComp.setBounds(bottomBounds.reduced(trim).toType<int>());
    panComp.setBounds(topRightBounds.reduced(trim).toType<int>());
    powerComp.setBounds(bottomRightBounds.reduced(trim).toType<int>());
}

void OscillatorPanel::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::white);
}
//===============================================================================
SoundSourcePanel::SoundSourcePanel(SynthParameterGroup* allParams, int index, apvts* tree, OctaneUpdater* updater) :
oscPanel(allParams->oscLevels[index],
         allParams->oscPositions[index],
         allParams->oscPans[index],
         allParams->oscPowers[index],
         updater,
         tree,
         index),
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
linkedTree(tree),
linkedUpdater(updater)
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
MacroPanel::MacroPanel(SynthParam* pitch, SynthParam* mod, SynthParam* key) :
pitchComp(pitch),
modComp(mod),
keyComp(key)
{
    addAndMakeVisible(&pitchComp);
    addAndMakeVisible(&modComp);
    addAndMakeVisible(&keyComp);
}

void MacroPanel::resized()
{
    auto iBounds = getLocalBounds();
    auto dX = iBounds.getWidth() / 3;
    auto dY = iBounds.getHeight();
    auto cushion = dX / 12;
    auto squareSide = (dX > dY) ? dY : dX;
    auto pBounds = juce::Rectangle<int>(0, dY / 2, squareSide, squareSide);
    auto mBounds = juce::Rectangle<int>(dX, dY / 2, squareSide, squareSide);
    auto kBounds = juce::Rectangle<int>(2 * dX, dY / 2, squareSide, squareSide);
    pitchComp.setBounds(pBounds.reduced(cushion));
    modComp.setBounds(mBounds.reduced(cushion));
    keyComp.setBounds(kBounds.reduced(cushion));
}

void MacroPanel::paint(juce::Graphics &g)
{
    auto fBounds = getLocalBounds().toFloat();
    auto keyBox = fBounds.removeFromTop(fBounds.getHeight() / 2);
    auto pitchBox = keyBox.removeFromLeft(fBounds.getWidth() / 3);
    auto modBox = keyBox.removeFromLeft(fBounds.getWidth() / 3);
    g.setColour(UXPalette::darkGray);
    g.drawFittedText("Pitch Wheel", pitchBox.toType<int>(), juce::Justification::left, 2, 0.6f);
    g.drawFittedText("Mod Wheel", modBox.toType<int>(), juce::Justification::left, 2, 0.6f);
    g.drawFittedText("Key Track", keyBox.toType<int>(), juce::Justification::left, 2, 0.6f);
}
//==============================================================================
OctaneEditor::OctaneEditor(SynthParameterGroup* pGroup, apvts* tree, OctaneUpdater* update) :
paramGroup(pGroup),
linkedTree(tree),
linkedUpdater(update),
filterPanel(&pGroup->filterCutoff, &pGroup->filterResonance, &pGroup->filterWetDry, update, tree),
macroPanel(&pGroup->pitchWheelValue, &pGroup->modWheelValue, &pGroup->keyTrackValue)
{
    for(int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        oscPanels.add(new SoundSourcePanel(paramGroup, i, tree, linkedUpdater));
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
    addAndMakeVisible(&filterPanel);
    addAndMakeVisible(&macroPanel);
}

void OctaneEditor::resized()
{
    auto fBounds = getLocalBounds().toFloat();
    auto bottomBounds = fBounds.removeFromBottom(fBounds.getHeight() / 6);
    auto lfoBounds = fBounds.removeFromRight(fBounds.getWidth() / 5);
    auto lfoHeight = lfoBounds.getHeight() / NUM_LFOS;
    for(int i = 0; i < NUM_LFOS; ++i)
    {
        if(i < NUM_LFOS - 1)
            lfoPanels[i]->setBounds(lfoBounds.removeFromTop(lfoHeight).toType<int>());
        else
            lfoPanels[i]->setBounds(lfoBounds.toType<int>());
    }
    auto leftfBounds = fBounds.removeFromLeft(fBounds.getWidth() / 2);
    oscPanels[0]->setBounds(leftfBounds.removeFromTop(fBounds.getHeight() / 2).toType<int>());
    oscPanels[1]->setBounds(fBounds.removeFromTop(fBounds.getHeight() / 2).toType<int>());
    oscPanels[2]->setBounds(leftfBounds.toType<int>());
    oscPanels[3]->setBounds(fBounds.toType<int>());
    auto dX = bottomBounds.getWidth() / 3;
    filterPanel.setBounds(bottomBounds.removeFromLeft(dX).toType<int>());
    macroPanel.setBounds(bottomBounds.removeFromRight(dX / 2).toType<int>());
}

void OctaneEditor::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::white);
}
