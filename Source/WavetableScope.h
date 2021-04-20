/*
  ==============================================================================

    WavetableScope.h
    Created: 19 Apr 2021 3:23:37pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RGBColor.h"
#include "GlobalColor.h"
#include "Synthesizer.h"
#define SCOPE_RESOLUTION 128

class WaveScope : public juce::Component, juce::Slider::Listener
{
public:
    WaveScope(std::vector<std::vector<float>>& data, juce::Slider* s, float pos = 0.0f);
    ~WaveScope() {};
    void paint(juce::Graphics& g) override;
    void setPosition(float pos); //recalculate colors in here;
    void sliderValueChanged(juce::Slider* s) override;
    void removeListener();
    void initPaths();
    void alterFor3d(juce::Path* p, float index)
    {
        auto fBounds = getBounds().toFloat();
        auto dX = fBounds.getWidth() / numTraces / 4;
        auto dY = fBounds.getHeight() / numTraces / 4;
        auto t = juce::AffineTransform::scale(0.55f, 0.55f).followedBy(juce::AffineTransform::shear(0.0f, 0.2f)).followedBy(juce::AffineTransform::translation((dX * index) + (fBounds.getWidth() / 8), -(dY * index * 0.7f) +  (fBounds.getHeight() / 5)));
        p->applyTransform(t);
    }
    void mouseDown(const juce::MouseEvent &m) override
    {
        fake3d = !fake3d;
        setPosition(position);
        repaint();
    }
    void updateCurrentTrace();
private:
    float minIncrement;
    std::vector<std::vector<float>> graphData;
    std::vector<juce::Colour> traceColors;
    int numTraces;
    int highlightIndex;
    bool fake3d;
    float position;
    float aExp;
    juce::Slider* slider;
    juce::OwnedArray<juce::Path> traces;
    juce::Path highlightedTrace2d;
    juce::Path highlightedTrace3d;
    std::array<float, SCOPE_RESOLUTION> currentValues;
};


class WaveScopeHolder : public juce::Component
{
public:
    WaveScopeHolder(std::vector<std::vector<float>> data, juce::Slider* s);
    void replace(std::vector<std::vector<float>> newData);
    void resized() override
    {
        scope->setBounds(getBounds());
    }
private:
    std::unique_ptr<WaveScope> scope;
    juce::Slider* slider;
};

class TableSelectorButton : public juce::ShapeButton
{
public:
    TableSelectorButton(bool isNext) :
    juce::ShapeButton("modButton", UXPalette::darkRed, UXPalette::darkRed, UXPalette::lightRed),
    facesRight(isNext)
    {
        setClickingTogglesState(true);
    }
    void paintButton(juce::Graphics& g, bool mouseOver, bool isMouseDown)
    {
        auto fBounds = getLocalBounds().toFloat();
        g.setColour(UXPalette::lightGray);
        g.fillRoundedRectangle(fBounds, 3.5f);
        auto dX = fBounds.getHeight() / 3.0f;
        auto half = fBounds.getHeight() / 2.0f;
        arrowPath.clear();
        if(facesRight)
        {
            arrowPath.startNewSubPath(dX, dX);
            arrowPath.lineTo(dX, 2.0f * dX);
            arrowPath.lineTo(2.0f * dX, half);
            arrowPath.closeSubPath();
        }
        if(!facesRight)
        {
            arrowPath.startNewSubPath(2.0f * dX, dX);
            arrowPath.lineTo(2.0f * dX, 2.0f * dX);
            arrowPath.lineTo(dX, half);
            arrowPath.closeSubPath();
        }
        g.setColour(UXPalette::darkGray);
        g.fillPath(arrowPath);
    }
private:
    juce::Path arrowPath;
    bool facesRight;
};


class TableSelector : public juce::Component, juce::Button::Listener, juce::ComboBox::Listener
{
public:
    TableSelector(WavetableSynth* s, WaveScopeHolder* w);
    void buttonClicked(juce::Button* b) override;
    void comboBoxChanged(juce::ComboBox* box) override;
    void resized() override;
private:
    TableSelectorButton nextButton;
    TableSelectorButton lastButton;
    juce::ComboBox waveSelect;
    WavetableSynth* pSynth;
    WaveScopeHolder* pScope;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> wAttach;
    juce::StringArray fileNames;
};
