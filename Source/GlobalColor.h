/*
  ==============================================================================

    GlobalColor.h
    Created: 9 Apr 2021 6:15:55pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "RGBColor.h"
namespace UXPalette {
static juce::Colour darkGray1 = Color::RGBColor(37, 50, 53);
static juce::Colour darkGray2 = Color::RGBColor(50, 80, 90);
static juce::Colour lightGray = Color::RGBColor(125, 126, 129);
static juce::Colour darkGray = lightGray.darker(0.7f);
static juce::Colour darkGray4 = lightGray.darker(0.9f);
static juce::Colour lightRed = Color::RGBColor(226, 76, 85);
static juce::Colour darkRed = lightRed.darker(0.45f);
static juce::Colour lightOrange = Color::RGBColor(249, 171, 62);
static juce::Colour highlight = Color::RGBColor(255, 236, 95);
static juce::Colour nearBlack = Color::RGBColor(24, 24, 25);
static juce::Font futuraMediumItalic(float size)
{
    return juce::Font("Futura Medium", size, 0).withStyle(juce::Font::FontStyleFlags::italic);
}
static juce::String operatorLabelFontName = "SquareFont Outline";
static juce::String modGridFontName = "Roboto Light";
static juce::Colour grayShadeL = Color::RGBColor(169, 179, 193);
static juce::Colour grayShadeD = grayShadeL.darker(0.3f);
static std::vector<juce::Colour> modTargetShades = Color::shadesBetweenVec(grayShadeD, grayShadeL, 5);
static juce::Colour thumbYellow = Color::RGBColor(255, 236, 95);
static juce::Colour thumbBlue = Color::RGBColor(32, 139, 181);

};

