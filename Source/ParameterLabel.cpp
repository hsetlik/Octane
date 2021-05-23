/*
  ==============================================================================

    ParameterLabel.cpp
    Created: 23 May 2021 11:08:49am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "ParameterLabel.h"
void ParamLabel::resized()
{
    switch(justif)
    {
        case AboveCenter:
        {
            setBounds(ComponentUtil::boxAbove(*linkedComp, 6));
            break;
        }
        case BelowCenter:
        {
            setBounds(ComponentUtil::boxBelow(*linkedComp, 6));
            break;
        }
    }
    
}

void ParamLabel::paint(juce::Graphics &g)
{
    auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::RobotoLightItalic_ttf, BinaryData::RobotoLightItalic_ttfSize);
    auto font = juce::Font(typeface);
    auto height = linkedComp->getHeight() / 6;
    font.setHeight((float)height);
    g.setFont(font);
    g.setColour(UXPalette::darkBkgnd);
    g.drawFittedText(labelText, getLocalBounds(), juce::Justification::centred, 1);
}

void ParamLabel::componentMovedOrResized(juce::Component &comp, bool moved, bool wasResized)
{
    resized();
    repaint();
}
