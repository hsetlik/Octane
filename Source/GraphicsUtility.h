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
    using fPoint = juce::Point<float>;
public:
    static fPoint centerPoint(juce::Rectangle<float>& rect)
    {
        auto centerX = rect.getX() + (rect.getWidth() / 2);
        auto centerY = rect.getY() + (rect.getHeight() / 2);
        return fPoint(centerX, centerY);
    }
    static juce::Path betweenEllipses(juce::Rectangle<float>& r1, juce::Rectangle<float>& r2)
    {
        juce::Path path;
        auto center1 = centerPoint(r1);
        auto radius1 = r1.getWidth() / 2.0f;
        auto center2 = centerPoint(r2);
        auto radius2 = r2.getWidth() / 2.0f;
        auto angle = center1.getAngleToPoint(center2);
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
    static fPoint lerp(fPoint a, fPoint b, float t)
    {
        return a + ((b - a) * t);
    }
    static float bezierAt(fPoint startPoint, fPoint endPoint, fPoint controlPoint, float t)
    {
        auto start = lerp(startPoint, controlPoint, t);
        auto end = lerp(controlPoint, endPoint, t);
        auto finalPoint = lerp(start, end, t);
        return finalPoint.y;
    }
    static fPoint bezierPointAt(fPoint startPoint, fPoint endPoint, fPoint controlPoint, float t)
    {
        auto start = lerp(startPoint, controlPoint, t);
        auto end = lerp(controlPoint, endPoint, t);
        return lerp(start, end, t);
    }
    static float bezierAt(fPoint startPoint, fPoint endPoint, fPoint c1, fPoint c2, float t)
    {
        auto point12 = lerp(startPoint, c1, t);
        auto point23 = lerp(c1, c2, t);
        auto point34 = lerp(c2, endPoint, t);
        auto p1 = lerp(point12, point23, t);
        auto p2 = lerp(point23, point34, t);
        return lerp(p1, p2, t).y;
    }
    static fPoint bezierPointAt(fPoint startPoint, fPoint endPoint, fPoint c1, fPoint c2, float t)
    {
        auto point12 = lerp(startPoint, c1, t);
        auto point23 = lerp(c1, c2, t);
        auto point34 = lerp(c2, endPoint, t);
        auto p1 = lerp(point12, point23, t);
        auto p2 = lerp(point23, point34, t);
        return lerp(p1, p2, t);
    }
    static bool distanceGreaterThan(fPoint p1, fPoint p2, float min)
    {
        return(fabs(p1.getDistanceFrom(p2)) > min);
    }
    
};

class ComponentUtil
{
public:
    static juce::Rectangle<int> offRightOf(juce::Component& parent, int width)
    {
        auto iBounds = parent.getBounds();
        auto x = iBounds.getRight();
        auto y = iBounds.getY();
        auto height = iBounds.getHeight();
        return juce::Rectangle<int>(x, y, width, height);
    }
    static juce::Rectangle<int> inTopRightCorner(juce::Component& parent, int width, int offset=0, int height=0)
    {
        if(height == 0)
            height = width;
        auto iBounds = parent.getBounds();
        auto x = iBounds.getRight() - width;
        auto y = iBounds.getY();
        return juce::Rectangle<int>(x - offset, y + offset, width, height);
    }
};
