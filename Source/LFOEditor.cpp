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
    {
        handles = nullptr;
        return nullptr;
    }
        
    else if(currentType == PointType::Bezier)
    {
        auto* output = new CenterHandle::CurveHandlePair(this, parentComp, false);
        auto xCenter = getCenter().x;
        auto yCenter = getCenter().y;
        auto frontX = (xCenter + 0.2f > 0.95f) ? 0.95f : xCenter + 0.2f;
        auto rearX = (xCenter - 0.2f < 0.05f) ? 0.05f : xCenter - 0.2f;
        output->front.setCenter(frontX, yCenter);
        output->rear.setCenter(rearX, yCenter);
        handles = output;
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
        handles = output;
        return output;
    }
}
//=================================================================

LFOEditor::LFOEditor(lfoArray* arr) :
startPoint(0.0f, 0.0f, false),
endPoint(1.0f, 0.0f, false),
linkedArray(arr),
frameIndex(0)
{
    addAndMakeVisible(centerHandles.add(new CenterHandle(this, PointType::Linear, 0.05f, 0.95f)));
    addAndMakeVisible(centerHandles.add(new CenterHandle(this, PointType::Linear, 0.5f, 0.05f)));
    addAndMakeVisible(centerHandles.add(new CenterHandle(this, PointType::Linear, 0.95f, 0.95f)));
    //! make sure the first and last points are uneditable
    centerHandles[0]->setInterceptsMouseClicks(false, false);
    centerHandles[2]->setInterceptsMouseClicks(false, false);
    startTimerHz(30);
}

void LFOEditor::timerCallback()
{
    ++frameIndex;
    if(frameIndex == 10) //! only need to recalculate the array once every 10 frames or so (every 1/3 second)
    {
        setArray();
        frameIndex = 0;
    }
    updateHandles();
    calculatePaths();
    repaint();
    
}

void LFOEditor::calculatePaths()
{
    auto numSections = centerHandles.size() - 1;
    paths.clear();
    for(int i = 0; i < numSections; ++i)
    {
        paths.add(new juce::Path());
        auto path = paths.getLast();
        path->startNewSubPath(centerHandles[i]->getBounds().getCentre().x, centerHandles[i]->getBounds().getCentre().y);
        //! determine which type pf path to draw
        if(centerHandles[i]->isSpline() && centerHandles[i + 1]->isSpline())
            //! draw a cubic bezier when both points are splines
        {
            auto c1 = centerHandles[i]->getLeadHandlePoint(this);
            auto c2 = centerHandles[i + 1]->getRearHandlePoint(this);
            path->cubicTo(c1, c2,
                          fPoint(centerHandles[i + 1]->getBounds().getCentre().x,
                                 centerHandles[i + 1]->getBounds().getCentre().y));
            
        }
        else if(!centerHandles[i]->isSpline() && !centerHandles[i + 1]->isSpline())
            //! draw a line when both points are linear
        {
            path->lineTo(centerHandles[i + 1]->getBounds().getCentre().x, centerHandles[i + 1]->getBounds().getCentre().y);
        }
        else if(centerHandles[i]->isSpline() && !centerHandles[i + 1]->isSpline())
            //! quatratic using the lead handle of the first point
        {
            auto c1 = centerHandles[i]->getLeadHandlePoint(this);
            path->quadraticTo(c1, fPoint(centerHandles[i + 1]->getBounds().getCentre().x, centerHandles[i + 1]->getBounds().getCentre().y));
        }
        else if(!centerHandles[i]->isSpline() && centerHandles[i + 1]->isSpline())
            //! quadratic using the rear handle of the second point
        {
            auto c1 = centerHandles[i + 1]->getRearHandlePoint(this);
            path->quadraticTo(c1, fPoint(centerHandles[i + 1]->getBounds().getCentre().x, centerHandles[i + 1]->getBounds().getCentre().y));
        }
    }
    for(auto pair : handlePairs)
    {
        paths.add(new juce::Path());
        auto path = paths.getLast();
        path->startNewSubPath(pair->front.getBounds().getCentre().x, pair->front.getBounds().getCentre().y);
        path->lineTo(pair->linkedCenter->getBounds().getCentre().x, pair->linkedCenter->getBounds().getCentre().y);
        path->lineTo(pair->rear.getBounds().getCentre().x, pair->rear.getBounds().getCentre().y);
    }
}

void LFOEditor::resized()
{
    for(auto p : centerHandles)
    {
        p->dX = getWidth() / 20;
        p->resized();
        auto relative = p->getCenter();
        //printf("Center At: %f, %f\n", relative.x, relative.y);
        //printf("Size: %d, %d\n", p->getWidth(), p->getHeight());
        p->setCentrePosition(proportionOfWidth(relative.x), proportionOfHeight(relative.y));
    }
    for(auto pair : handlePairs)
    {
        pair->front.dX = getWidth() / 20;
        pair->rear.dX = getWidth() / 20;
        auto rFront = pair->front.getCenter();
        pair->front.setCenter(rFront.x, rFront.y, this);
        auto rRear = pair->rear.getCenter();
        pair->rear.setCenter(rRear.x, rRear.y, this);
        pair->front.resized();
        pair->rear.resized();
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

void LFOEditor::updateHandles()
{
    for(auto center : centerHandles)
    {
        if(center->needsHandles)
        {
            setHandlesFor(center);
        }
    }
}

void LFOEditor::setHandlesFor(CenterHandle *center)
{
    //! go through and delete any existing handles for this center
    for(auto pair : handlePairs)
    {
        if(pair->linkedCenter == center)
        {
            handlePairs.removeObject(pair);
            break;
        }
    }
    //! now make the new pair
    handlePairs.add(center->makeHandlePair());
    auto newPair = handlePairs.getLast();
    addAndMakeVisible(newPair->front);
    addAndMakeVisible(newPair->rear);
    resized();
}

