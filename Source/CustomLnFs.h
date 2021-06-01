/*
  ==============================================================================

    CustomLnFs.h
    Created: 5 Mar 2021 7:43:39am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RgbColor.h"
class ModSystemLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModSystemLookAndFeel()
    {
    
    }
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& s) override;
    void drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider &) override;
};

class DepthSliderLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DepthSliderLookAndFeel()
    {
        
    }
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& s) override;
    void drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider &) override;
};

class SynthSourceLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SynthSourceLookAndFeel();
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& s) override;
};

class DropdownLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DropdownLookAndFeel()
    {
        
    }
    void drawButtonBackground(juce::Graphics& g, juce::Button& b, const juce::Colour& bColor, bool isHighlighted, bool isDown) override;
    void drawButtonText(juce::Graphics& g, juce::TextButton& b, bool isHighlighted, bool isDown) override;
    int getTextButtonWidthToFitText(juce::TextButton& b, int height) override;
};

class OctaneLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OctaneLookAndFeel()
    {
    }
    void drawButtonBackground(juce::Graphics& g, juce::Button& b, const juce::Colour& bColor, bool isHighlighted, bool isDown) override;
    void drawButtonText(juce::Graphics& g, juce::TextButton& b, bool isHighlighted, bool isDown) override;
    int getTextButtonWidthToFitText(juce::TextButton& b, int height) override;
};
