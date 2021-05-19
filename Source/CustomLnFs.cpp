/*
  ==============================================================================

    CustomLnFs.cpp
    Created: 5 Mar 2021 7:43:39am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "CustomLnFs.h"

void ModSystemLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& s)
{
    auto iBounds = juce::Rectangle<int> {x, y, width, height};
    auto fBounds = iBounds.toFloat();
    g.setColour(UXPalette::darkGray);
    g.fillEllipse(fBounds);
    g.setColour(UXPalette::lightGray);
    auto angle = fabs(rotaryStartAngle - rotaryEndAngle) * sliderPos;
    auto centerX = fBounds.getX() + (fBounds.getWidth() / 2.0f);
    auto centerY = fBounds.getY() + (fBounds.getHeight() / 2.0f);
    auto radius = fBounds.getWidth() * 0.4f;
    auto strokeType = juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    juce::Path track;
    track.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.strokePath(track, strokeType);
    g.setColour(UXPalette::highlight);
    auto iRadius = radius * 0.6f;
    juce::Path thumb;
    thumb.startNewSubPath(centerX, centerY - radius);
    thumb.lineTo(centerX, centerY - iRadius);
    thumb.closeSubPath();
    thumb.applyTransform(juce::AffineTransform::rotation(rotaryStartAngle + angle, centerX, centerY));
    g.strokePath(thumb, strokeType);
}

void ModSystemLookAndFeel::drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos
                            , float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider &slider)
{
    auto fPos = 1.0f - (sliderPos / maxSliderPos);
    auto fBounds = slider.getLocalBounds().toFloat();
    auto bkgndWidth = fBounds.getWidth() * 0.2f;
    auto xOffsetBkgnd = (fBounds.getWidth() / 2.0f) - (bkgndWidth / 2.0f);
    auto corner = bkgndWidth / 3.5f;
    auto thumbWidth = fBounds.getWidth() * 0.45f;
    
    auto thumbXOffset = (fBounds.getWidth() / 2.0f) - (thumbWidth / 2.0f);
    auto thumbHeight = (fBounds.getHeight() - slider.getTextBoxHeight()) * 0.1f;
    auto thumbY = (1.0f - fPos) * ((fBounds.getHeight() - slider.getTextBoxHeight()) - thumbHeight - 5);
    thumbY += 5;
    //draw the background
    g.setColour(UXPalette::darkBkgnd);
    g.fillRoundedRectangle(x + xOffsetBkgnd,
                           5,
                           bkgndWidth,
                           fBounds.getHeight() - (slider.getTextBoxHeight() * 1.2f) - 5,
                           corner);
    //draw the thumb
    g.setColour(UXPalette::highlight);
    g.fillRoundedRectangle(x + thumbXOffset,
                           thumbY,
                           thumbWidth,
                           thumbHeight,
                           corner);
}

//===========================================================================================================
void DepthSliderLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &s)
{
    auto iBounds = juce::Rectangle<int> {x, y, width, height};
    auto fBounds = iBounds.toFloat();
    auto angle = fabs(rotaryStartAngle - rotaryEndAngle) * sliderPos;
    auto midAngle = (fabs(rotaryStartAngle - rotaryEndAngle)  /  2.0f) + rotaryStartAngle;
    auto strokeType = juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    auto centerX = fBounds.getX() + (fBounds.getWidth() / 2.0f);
    auto centerY = fBounds.getY() + (fBounds.getHeight() / 2.0f);
    auto radius = fBounds.getWidth() * 0.45f;
    juce::Path track;
    track.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(UXPalette::lightGray);
    g.strokePath(track, strokeType);
    juce::Path thumb;
    
    g.setColour(UXPalette::highlight);
    if(s.getValue() < 0.0f)
    {
        g.setColour(UXPalette::thumbBlue);
        thumb.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle + angle,  midAngle, true);
    }
    else
        thumb.addCentredArc(centerX, centerY, radius, radius, 0.0f, midAngle, rotaryStartAngle + angle, true);
    g.strokePath(thumb, strokeType);
}

void DepthSliderLookAndFeel::drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider &slider)
{
    
    auto fPos = 1.0f - (sliderPos / maxSliderPos);
    auto fBounds = slider.getBounds().toFloat();
    auto bkgndWidth = fBounds.getWidth() * 0.2f;
    auto xOffsetBkgnd = (fBounds.getWidth() / 2.0f) - (bkgndWidth / 2.0f);
    auto corner = bkgndWidth / 3.5f;
    auto thumbWidth = fBounds.getWidth() * 0.45f;
    
    auto thumbXOffset = (fBounds.getWidth() / 2.0f) - (thumbWidth / 2.0f);
    auto thumbHeight = (fBounds.getHeight() - slider.getTextBoxHeight()) * 0.1f;
    auto thumbY = (1.0f - fPos) * ((fBounds.getHeight() - slider.getTextBoxHeight()) - thumbHeight - 5);
    
    thumbY += 5;
    //draw the background
    g.setColour(UXPalette::darkBkgnd);
    g.fillRoundedRectangle(x + xOffsetBkgnd,
                           5,
                           bkgndWidth,
                           fBounds.getHeight() - (slider.getTextBoxHeight() * 1.2f) - 5,
                           corner);
    //draw the thumb
    g.setColour(UXPalette::highlight);
    g.fillRoundedRectangle(x + thumbXOffset,
                           thumbY,
                           thumbWidth,
                           thumbHeight,
                           corner);
    
}

SynthSourceLookAndFeel::SynthSourceLookAndFeel()
{
    
}

void SynthSourceLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &s)
{
    auto iBounds = juce::Rectangle<int> {x, y, width, height};
    auto fBounds = iBounds.toFloat();
    g.setColour(UXPalette::darkGray);
    g.fillEllipse(fBounds);
    g.setColour(UXPalette::lightGray);
    auto angle = fabs(rotaryStartAngle - rotaryEndAngle) * sliderPos;
    auto centerX = fBounds.getX() + (fBounds.getWidth() / 2.0f);
    auto centerY = fBounds.getY() + (fBounds.getHeight() / 2.0f);
    auto radius = fBounds.getWidth() * 0.4f;
    auto strokeType = juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    juce::Path track;
    track.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.strokePath(track, strokeType);
    g.setColour(UXPalette::highlight);
    auto iRadius = radius * 0.6f;
    juce::Path thumb;
    thumb.startNewSubPath(centerX, centerY - radius);
    thumb.lineTo(centerX, centerY - iRadius);
    thumb.closeSubPath();
    thumb.applyTransform(juce::AffineTransform::rotation(rotaryStartAngle + angle, centerX, centerY));
    g.strokePath(thumb, strokeType);
};

void DropdownLookAndFeel::drawButtonBackground(juce::Graphics &g, juce::Button &b, const juce::Colour &bColor, bool isHighlighted, bool isDown)
{
    b.resized();
    auto fBounds = b.getLocalBounds().toFloat();
    auto corner = fBounds.getHeight() / 6.0f;
    g.setColour(UXPalette::lightGray);
    g.fillRoundedRectangle(fBounds, corner);
    auto triBounds = fBounds.removeFromRight(fBounds.getHeight()).reduced(2.0f * corner);
    juce::Path triangle;
    triangle.startNewSubPath(triBounds.getX(), triBounds.getY());
    triangle.lineTo(triBounds.getRight(), triBounds.getY());
    triangle.lineTo(triBounds.getX() + (triBounds.getWidth() / 2.0f), triBounds.getBottom());
    triangle.closeSubPath();
    g.setColour(juce::Colours::white);
    g.fillPath(triangle);
}

void DropdownLookAndFeel::drawButtonText(juce::Graphics &g, juce::TextButton &b, bool isHighlighted, bool isDown)
{
    auto fBounds = b.getLocalBounds().toFloat();
    auto textHeight = (int)fBounds.getHeight() * 0.8f;
    auto font = getTextButtonFont(b, textHeight);
    g.setColour(juce::Colours::white);
    g.setFont(font);
    auto iBounds = b.getLocalBounds();
    auto iX = iBounds.getX();
    auto iY = iBounds.getY();
    auto iWidth = iBounds.getWidth();
    auto dX = iWidth / 11;
    auto textBox = juce::Rectangle<int>(iX + dX, iY, 8 * dX, iBounds.getHeight());
    g.drawText(b.getButtonText(), textBox, juce::Justification::left);
}

int DropdownLookAndFeel::getTextButtonWidthToFitText(juce::TextButton &b, int height)
{
    auto font = getTextButtonFont(b, height);
    auto str = b.getButtonText();
    auto textWidth = font.getStringWidth(str);
    auto dX = (float)textWidth / 8.0f;
    return (int)(14 * dX);
}
