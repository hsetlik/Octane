/*
  ==============================================================================

    LFOEditor.cpp
    Created: 6 May 2021 5:37:07pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "LFOEditor.h"


LFOShape::LFOShape()
{
    
}

void LFOShape::applyToArray()
{
    
}
//=================================================================
void CenterHandle::paint(juce::Graphics &g)
{
    outline.clear();
    switch(currentType)
    {
        case PointType::Linear: //! linear points are represented by squares
        {
            outline.addRectangle(getLocalBounds().toFloat());
            break;
        }
        case PointType::Bezier: //! linked spline points are circles
        {
            outline.addEllipse(getLocalBounds().toFloat());
            break;
        }
        case PointType::Split: //! split splines are diamonds
        {
            auto fBounds = getLocalBounds().toFloat();
            auto width = fBounds.getWidth();
            auto height = fBounds.getWidth();
            outline.startNewSubPath(0.0f, height / 2.0f);
            outline.lineTo(width / 2.0f, height);
            outline.lineTo(width, height / 2.0f);
            outline.lineTo(width / 2.0f, 0.0f);
            outline.closeSubPath();
            break;
        }
    }
    g.setColour(UXPalette::lightGray);
    g.fillPath(outline);
}

void CenterHandle::nextHandleType()
{
    if(currentType != PointType::Split)
    {
        currentType = (PointType)((int)currentType + 1);
    }
    else
    {
        currentType = PointType::Linear;
    }
    repaint();
    if(currentType != PointType::Linear)
        needsHandles = true;
}

CenterHandle::CurveHandlePair* CenterHandle::makeHandlePair()
{
    needsHandles = false;
    if(currentType == PointType::Linear)
        return nullptr;
    else if(currentType == PointType::Bezier)
    {
        auto* output = new CenterHandle::CurveHandlePair(this, parentComp, false);
        auto xCenter = getCenter().x;
        auto yCenter = getCenter().y;
        auto frontX = (xCenter + 0.2f > 0.95f) ? 0.95f : xCenter + 0.2f;
        auto rearX = (xCenter - 0.2f < 0.05f) ? 0.05f : xCenter - 0.2f;
        output->front.setCenter(frontX, yCenter);
        output->rear.setCenter(rearX, yCenter);
        return output;
    }
    else
    {
        auto* output = new CenterHandle::CurveHandlePair(this, parentComp, true);
        auto xCenter = getCenter().x;
        auto yCenter = getCenter().y;
        if(yCenter - 0.2f > 0.05f)
            yCenter -= 0.2f;
        auto frontX = (xCenter + 0.2f > 0.95f) ? 0.95f : xCenter + 0.2f;
        auto rearX = (xCenter - 0.2f < 0.05f) ? 0.05f : xCenter - 0.2f;
        output->front.setCenter(frontX, yCenter);
        output->rear.setCenter(rearX, yCenter);
        return output;
    }
}
//=================================================================

LFOEditor::LFOEditor(lfoArray* arr) :
startPoint(0.0f, 0.0f, false),
endPoint(1.0f, 0.0f, false),
linkedArray(arr)
{
    addAndMakeVisible(pointHandles.add(new CenterHandle(this, PointType::Linear, 0.05f, 0.95f)));
    addAndMakeVisible(pointHandles.add(new CenterHandle(this, PointType::Linear, 0.5f, 0.05f)));
    addAndMakeVisible(pointHandles.add(new CenterHandle(this, PointType::Linear, 0.95f, 0.95f)));
    //! make sure the first and last points are uneditable
    pointHandles[0]->setInterceptsMouseClicks(false, false);
    pointHandles[2]->setInterceptsMouseClicks(false, false);
    startTimerHz(30);
}

void LFOEditor::timerCallback()
{
    calculatePaths();
    repaint();
}

void LFOEditor::calculatePaths()
{
    auto numSections = pointHandles.size() - 1;
    paths.clear();
    for(int i = 0; i < numSections; ++i)
    {
        paths.add(new juce::Path());
        auto path = paths.getLast();
        path->startNewSubPath(pointHandles[i]->getBounds().getCentre().x, pointHandles[i]->getBounds().getCentre().y);
        path->lineTo(pointHandles[i + 1]->getBounds().getCentre().x, pointHandles[i + 1]->getBounds().getCentre().y);
    }
}

void LFOEditor::resized()
{
    for(auto p : pointHandles)
    {
        p->dX = getWidth() / 20;
        p->resized();
        auto relative = p->getCenter();
        //printf("Center At: %f, %f\n", relative.x, relative.y);
        //printf("Size: %d, %d\n", p->getWidth(), p->getHeight());
        p->setCentrePosition(proportionOfWidth(relative.x), proportionOfHeight(relative.y));
    }
}

void LFOEditor::paint(juce::Graphics &g)
{
    g.fillAll(UXPalette::darkBkgnd);
    auto stroke = juce::PathStrokeType(1.0f);
    g.setColour(UXPalette::highlight);
    for(auto path : paths)
    {
        g.strokePath(*path, stroke);
    }
}
void LFOEditor::connectPoints(LFOPoint* first, LFOPoint* second)
{
    
}
void LFOEditor::setArray()
{
    auto lastPoint = &startPoint;
    for(auto point : points)
    {
        connectPoints(lastPoint, point);
        lastPoint = point;
    }
    connectPoints(lastPoint, &endPoint);
}

