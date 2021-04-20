/*
  ==============================================================================

    WavetableScope.cpp
    Created: 19 Apr 2021 3:23:37pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "WavetableScope.h"

WaveScope::WaveScope(std::vector<std::vector<float>>& data, juce::Slider* s, float pos) :
graphData(data),
numTraces((int)data.size()),
position(pos),
slider(s)
{
    slider->addListener(this);
    for(int i = 0; i < numTraces; ++i)
    {
        traceColors.push_back(juce::Colours::black);
        traces.add(new juce::Path());
    }
    minIncrement = 1.0f / (numTraces * 1.5f); //! this determines how frequently the scope should be redrawn as the slider moves
    initPaths();
    setPosition(pos);
}
void WaveScope::removeListener()
{
    slider->removeListener(this);
}
void WaveScope::initPaths()
{
    auto fBounds = getBounds().toFloat();
    auto y0 = fBounds.getHeight() / 2.0f;
    auto amplitude = y0 * 0.75f;
    auto dX = fBounds.getWidth() / SCOPE_RESOLUTION;
    for(int p = (numTraces - 1); p >= 0; --p)
    {
        traces[p]->clear();
        traces[p]->startNewSubPath(0.0f, fBounds.getBottom());
        traces[p]->lineTo(0.0f, y0);
        for(int i = 0; i < SCOPE_RESOLUTION; ++i)
        {
            traces[p]->lineTo(dX * i, y0 + (graphData[p][i] * amplitude));
        }
        traces[p]->lineTo(fBounds.getRight(), fBounds.getBottom());
        traces[p]->closeSubPath();
        alterFor3d(traces[p], (float)p);
    }
}
void WaveScope::setPosition(float pos)
{
    position = pos;
    aExp = 0.95f;
    highlightIndex = (int)position * numTraces;
    for(int i = 0; i < numTraces; ++i)
    {
        auto diff = fabs(i - (position * (numTraces - 1)));
        auto alpha = 1.0f * pow(aExp, diff);
        auto col = Color::blendHSB(UXPalette::highlight, UXPalette::nearBlack, alpha * 0.75f);
        traceColors[i] = col;
    }
    auto lowerIdx = floor(position * (numTraces - 1) * 0.99f);
    auto upperIdx = lowerIdx + 1;
    auto& upperData = graphData[upperIdx];
    auto& lowerData = graphData[lowerIdx];
    auto skew = (position * (numTraces - 1) * 0.99f) - (float)lowerIdx;
    for(auto i : currentValues)
        i = lowerData[i] + ((upperData[i] - lowerData[i]) * skew);
    updateCurrentTrace();
}
void WaveScope::sliderValueChanged(juce::Slider *s)
{
    if(fabs(s->getValue() - position) > minIncrement)
    {
        setPosition(s->getValue());
        repaint();
    }
}
void WaveScope::updateCurrentTrace()
{
    highlightedTrace2d.clear();
    auto fBounds = getBounds().toFloat();
    auto y0 = fBounds.getHeight() / 2.0f;
    auto dX = fBounds.getWidth() / SCOPE_RESOLUTION;
    highlightedTrace2d.startNewSubPath(0.0f, fBounds.getBottom());
    highlightedTrace2d.lineTo(0.0f, y0);
    auto idx = 0;
    auto amplitude = y0 * 0.75f;
    for(auto i : currentValues)
    {
        highlightedTrace2d.lineTo(idx * dX, y0 + (i * amplitude));
        ++idx;
    }
    highlightedTrace2d.lineTo(fBounds.getRight(), fBounds.getBottom());
    highlightedTrace2d.closeSubPath();
    highlightedTrace3d = highlightedTrace2d;
    alterFor3d(&highlightedTrace3d, position * SCOPE_RESOLUTION);
}

void WaveScope::paint(juce::Graphics &g)
{
    auto strokeType = juce::PathStrokeType(1.0f);
    g.fillAll(UXPalette::nearBlack);
    if(fake3d)
    {
        for(int p = (numTraces - 1); p >= 0; --p)
        {
            g.setColour(traceColors[p]);
            g.strokePath(*traces[p], strokeType);
            if(p == highlightIndex)
            {
                g.setColour(UXPalette::highlight);
                g.strokePath(highlightedTrace3d, strokeType);
            }
        }
    }
    else
    {
        g.setColour(UXPalette::highlight);
        g.strokePath(highlightedTrace2d, strokeType);
    }
}

WaveScopeHolder::WaveScopeHolder(std::vector<std::vector<float>> data, juce::Slider* s) :
scope(std::make_unique<WaveScope>(data, s)),
slider(s)
{
    addAndMakeVisible(*scope);
}

void WaveScopeHolder::replace(std::vector<std::vector<float> > newData)
{
    scope->setEnabled(false);
    scope->setVisible(false);
    scope->removeListener();
    scope.reset(new WaveScope(newData, slider));
    addAndMakeVisible(*scope);
    resized();
}

TableSelector::TableSelector(WavetableSynth* s, WaveScopeHolder* w, juce::ComboBox::Listener* list) :
nextButton(true),
lastButton(false),
pSynth(s),
pScope(w)
{
    waveSelect.addListener(list);
    fileNames = pSynth->getTableNames();
    nextButton.addListener(this);
    lastButton.addListener(this);
    waveSelect.addItemList(fileNames, 1);
    wAttach.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(*pSynth->getTree(), "wavetableParam", waveSelect));
    addAndMakeVisible(&nextButton);
    addAndMakeVisible(&lastButton);
    addAndMakeVisible(&waveSelect);
}
void TableSelector::buttonClicked(juce::Button *b)
{
    int startingIndex = waveSelect.getSelectedItemIndex();
    if(b == &nextButton && startingIndex < (waveSelect.getNumItems() - 1))
    {
        waveSelect.setSelectedItemIndex(startingIndex + 1);
    }
    if(b == &lastButton && startingIndex > 0)
    {
        waveSelect.setSelectedItemIndex(startingIndex - 1);
    }
}

void TableSelector::resized()
{
    auto dX = getWidth() / 9;
    lastButton.setBounds(0, 0, dX, dX);
    nextButton.setBounds(dX, 0, dX, dX);
    waveSelect.setBounds(2 * dX, 0, 7 * dX, dX);
}
