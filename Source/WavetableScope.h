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
#define SCOPE_RESOLUTION 128

class WaveScope : public juce::Component, juce::Slider::Listener
{
public:
    WaveScope(std::vector<std::vector<float>> data, juce::Slider* s, float pos = 0.0f);
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
