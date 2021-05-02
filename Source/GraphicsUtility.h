/*
  ==============================================================================

    GraphicsUtility.h
    Created: 2 May 2021 12:24:58am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class PathUtility
{
public:
    static juce::Point<float> centerPoint(juce::Rectangle<float>& rect)
    {
        auto centerX = rect.getX() + (rect.getWidth() / 2);
        auto centerY = rect.getY() + (rect.getHeight() / 2);
        return juce::Point<float>(centerX, centerY);
    }
    static juce::Path betweenEllipses(juce::Rectangle<float>& r1, juce::Rectangle<float>& r2)
    {
        juce::Path path;
        auto center1 = centerPoint(r1);
        auto radius1 = r1.getWidth() / 2.0f;
        auto center2 = centerPoint(r2);
        auto radius2 = r2.getWidth() / 2.0f;
        auto angle = std::asin((center1.getY() - center2.getY()) - (center1.getX() - center2.getX()));
        auto ninety = juce::MathConstants<float>::halfPi;
        auto lowerCorner1 = center1.getPointOnCircumference(radius1, angle + ninety);
        auto upperCorner1 = center1.getPointOnCircumference(radius1, angle - ninety);
        auto lowerCorner2 = center2.getPointOnCircumference(radius2, angle + ninety);
        auto upperCorner2 = center2.getPointOnCircumference(radius2, angle - ninety);
        
        path.startNewSubPath(lowerCorner1);
        path.lineTo(lowerCorner2);
        path.lineTo(upperCorner2);
        path.lineTo(upperCorner1);
        
        path.closeSubPath();
        
        return path;
    }
    
};
