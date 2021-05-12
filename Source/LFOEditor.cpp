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
    auto startType = currentType;
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
    else
        needsHandles = false;
    if(startType != PointType::Linear && currentType == PointType::Linear)
        needsHandlesRemoved = true;
    else
        needsHandlesRemoved = false;
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
        output->front->setCenter(frontX, yCenter);
        output->rear->setCenter(rearX, yCenter);
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
        output->front->setCenter(frontX, yCenter);
        output->rear->setCenter(rearX, yCenter);
        handles = output;
        return output;
    }
}

void CenterHandle::CurveHandlePair::componentMovedOrResized(juce::Component &comp, bool wasMoved, bool wasResized)
{
    //!  move the unclicked handle to match the clicked one relative to the center (if not split ofc)
    if(!isSplit)
    {
        /*
        auto idealRear = idealRearPos();
        auto idealFront = idealFrontPos();
        auto currentRear = rear.getCenter();
        auto currentFront = front.getCenter();
        if(&comp == &front)
        {
            if(PathUtility::distanceGreaterThan(currentRear, idealRear, MIN_POINT_DIFFERENCE))
            {
                rear.setCenter(idealRear.x, idealRear.y, parentComp);
            }
        }
        else if(&comp == &rear)
        {
            if(PathUtility::distanceGreaterThan(currentFront, idealFront, MIN_POINT_DIFFERENCE))
            {
                front.setCenter(idealFront.x, idealFront.y, parentComp);
            }
        }
         */
    }
}

fPoint CenterHandle::CurveHandlePair::idealRearPos()
{
    auto center = linkedCenter->getCenter();
    auto fCenter = front->getCenter();
    auto delta = fCenter - center;
    return center - delta;
}
fPoint CenterHandle::CurveHandlePair::idealFrontPos()
{
    auto center = linkedCenter->getCenter();
    auto rCenter = rear->getCenter();
    auto delta = center - rCenter;
    return center + delta;
}
//=================================================================
void AddPointButton::paintButton(juce::Graphics &g, bool isHighlighted, bool isDown)
{
    auto fBounds = getLocalBounds().toFloat();
    auto corner = fBounds.getHeight() / 4.0f;
    //outline.clear();
    g.setColour(UXPalette::lightGray);
    if(isDown)
        g.setColour(UXPalette::darkGray);
    g.fillRoundedRectangle(fBounds, corner);
    g.setColour(UXPalette::highlight);
    g.fillPath(symbol);
}
void AddPointButton::resized() //! use the \c resized methods to lay out the symbol paths
{
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getWidth() / 16;
    auto dY = fBounds.getHeight() / 8;
    symbol.clear();
    symbol.startNewSubPath(5 * dX, 3 * dY);
    symbol.lineTo(7 * dX, 3 * dY);
    symbol.lineTo(7 * dX, 1 * dY);
    symbol.lineTo(9 * dX, 1 * dY);
    symbol.lineTo(9 * dX, 3 * dY);
    symbol.lineTo(11 * dX, 3 * dY);
    symbol.lineTo(11 * dX, 5 * dY);
    symbol.lineTo(9 * dX, 5 * dY);
    symbol.lineTo(9 * dX, 7 * dY);
    symbol.lineTo(7 * dX, 7 * dY);
    symbol.lineTo(7 * dX, 5 * dY);
    symbol.lineTo(5 * dX, 5 * dY);
    symbol.closeSubPath();
    
}
void RemovePointButton::paintButton(juce::Graphics &g, bool isHighlighted, bool isDown)
{
    auto fBounds = getLocalBounds().toFloat();
    auto corner = fBounds.getHeight() / 4.0f;
    //outline.clear();
    g.setColour(UXPalette::lightGray);
    if(isDown)
        g.setColour(UXPalette::darkGray);
    g.fillRoundedRectangle(fBounds, corner);
    g.setColour(UXPalette::highlight);
    g.fillPath(symbol);
}
void RemovePointButton::resized()
{
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getWidth() / 16;
    auto dY = fBounds.getHeight() / 8;
    symbol.clear();
    symbol.startNewSubPath(5 * dX, 3 * dY);
    symbol.lineTo(11 * dX, 3 * dY);
    symbol.lineTo(11 * dX, 5 * dY);
    symbol.lineTo(5 * dX, 5 * dY);
    symbol.closeSubPath();
}

//===================================================================

LFOEditor::LFOEditor(lfoArray* arr) :
startPoint(0.0f, 0.0f, false),
endPoint(1.0f, 0.0f, false),
linkedArray(arr),
frameIndex(0)
{
    for(auto point : dataArray)
    {
        point = 1.0f;
    }
    addAndMakeVisible(centerHandles.add(new CenterHandle(this, PointType::Linear, 0.05f, 0.95f)));
    addAndMakeVisible(centerHandles.add(new CenterHandle(this, PointType::Linear, 0.5f, 0.05f)));
    addAndMakeVisible(centerHandles.add(new CenterHandle(this, PointType::Linear, 0.95f, 0.95f)));
    //! make sure the first and last points are uneditable
    centerHandles[0]->setInterceptsMouseClicks(false, false);
    centerHandles[2]->setInterceptsMouseClicks(false, false);
    startTimerHz(REPAINT_FPS);
}

LFOEditor::~LFOEditor()
{
    printf("LFO START\n");
    int idx = 0;
    for(auto point : dataArray)
    {
        printf("Point %d: %f\n", idx, point);
        ++idx;
    }
    printf("LFO FINISHED\n");
}

void LFOEditor::timerCallback()
{
    ++frameIndex;
    if(frameIndex == 15) //! only recalculate the array once every 15 frames or so (every 1/2 second) to save on CPU
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
        if(centerHandles[i]->isBezier() && centerHandles[i + 1]->isBezier())
            //! draw a cubic bezier when both points are splines
        {
            auto c1 = centerHandles[i]->getLeadHandlePoint(this);
            auto c2 = centerHandles[i + 1]->getRearHandlePoint(this);
            path->cubicTo(c1, c2,
                          fPoint(centerHandles[i + 1]->getBounds().getCentre().x,
                                 centerHandles[i + 1]->getBounds().getCentre().y));
            
        }
        else if(!centerHandles[i]->isBezier() && !centerHandles[i + 1]->isBezier())
            //! draw a line when both points are linear
        {
            path->lineTo(centerHandles[i + 1]->getBounds().getCentre().x, centerHandles[i + 1]->getBounds().getCentre().y);
        }
        else if(centerHandles[i]->isBezier() && !centerHandles[i + 1]->isBezier())
            //! quatratic using the lead handle of the first point
        {
            auto c1 = centerHandles[i]->getLeadHandlePoint(this);
            path->quadraticTo(c1, fPoint(centerHandles[i + 1]->getBounds().getCentre().x, centerHandles[i + 1]->getBounds().getCentre().y));
        }
        else if(!centerHandles[i]->isBezier() && centerHandles[i + 1]->isBezier())
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
        path->startNewSubPath(pair->front->getBounds().getCentre().x, pair->front->getBounds().getCentre().y);
        path->lineTo(pair->linkedCenter->getBounds().getCentre().x, pair->linkedCenter->getBounds().getCentre().y);
        path->lineTo(pair->rear->getBounds().getCentre().x, pair->rear->getBounds().getCentre().y);
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
        pair->front->dX = getWidth() / 20;
        pair->rear->dX = getWidth() / 20;
        auto rFront = pair->front->getCenter();
        pair->front->setCenter(rFront.x, rFront.y, this);
        auto rRear = pair->rear->getCenter();
        pair->rear->setCenter(rRear.x, rRear.y, this);
        pair->front->resized();
        pair->rear->resized();
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
void LFOEditor::connectPoints(CenterHandle* first, CenterHandle* second)
{
    float t = 0.0f;
    auto center1 = first->getCenter();
    auto center2 = second->getCenter();
    auto numPoints = (int)floor(center2.x * (float)LFO_POINTS) - floor(center1.x * (float)LFO_POINTS);
    auto idx = floor(center1.x * (float)LFO_POINTS);
    auto dT = 1.0f / numPoints;
    if(!first->isBezier() && !second->isBezier()) //! both linear
    {
        auto yStart = center1.y;
        auto yEnd = center2.y;
        for(int i = idx; i < numPoints; ++i)
        {
            t += dT;
            dataArray[i] = yStart + ((yEnd - yStart) * t);
        }
        return;
    }
    else if(first->isBezier() && second->isBezier()) //! both splie
    {
        auto c1 = first->handles->front->getCenter();
        auto c2 = second->handles->rear->getCenter();
        for(int i = idx; i < numPoints; ++i)
        {
            t += dT;
            dataArray[i] = PathUtility::bezierAt(center1, center2, c1, c2, t);
        }
        return;
    }
    else if(!first->isBezier() && second->isBezier()) //! only second is spline
    {
        auto c1 = second->handles->rear->getCenter();
        for(int i = idx; i < numPoints; ++i)
        {
            t += dT;
            dataArray[i] = PathUtility::bezierAt(center1, center2, c1, t);
        }
        return;
    }
    else if(first->isBezier() && !second->isBezier()) //! only first is spline
    {
        auto c1 = first->handles->front->getCenter();
        for(int i = idx; i < numPoints; ++i)
        {
            t += dT;
            dataArray[i] = PathUtility::bezierAt(center1, center2, c1, t);
        }
        return;
    }
}
void LFOEditor::setArray()
{
    auto* lastPoint = centerHandles[0];
    for(int i = 1; i < centerHandles.size(); ++i)
    {
        auto* newPoint = centerHandles[i];
        connectPoints(lastPoint, newPoint);
        lastPoint = newPoint;
    }
    *linkedArray = dataArray;
}

void LFOEditor::updateHandles()
{
    for(auto center : centerHandles)
    {
        if(center->needsHandles)
            setHandlesFor(center);
        if(center->needsHandlesRemoved)
            removeHandlesFrom(center);
        
    }
}

void LFOEditor::setHandlesFor(CenterHandle *center)
{
    //! go through and delete any existing handles for this center
    removeHandlesFrom(center);
    //! now make the new pair
    handlePairs.add(center->makeHandlePair());
    auto newPair = handlePairs.getLast();
    newPair->makeVisible();
    resized();
}

void LFOEditor::addPoint()
{
    //! 1: determine all the gaps between points
    std::vector<PointGap> gaps;
    for(int i = 1; i < centerHandles.size(); ++i)
    {
        auto size = centerHandles[i]->relativeX() - centerHandles[i - 1]->relativeX();
        gaps.push_back(PointGap(i - 1, i, size));
    }
    //! 2: find the largest one
    auto gap = largestGap(gaps);
    auto xValue = centerHandles[gap.lowerIdx]->relativeX() + (gap.gapSize / 2.0f);
    auto yValue = dataArray[floor(xValue * LFO_POINTS)];
    addAndMakeVisible(centerHandles.insert(gap.upperIdx, new CenterHandle(this, PointType::Linear, xValue, yValue)));
    resized();
}
void LFOEditor::removePoint()
{
    if(centerHandles.size() < 4)
        return;
    else
    {
        auto* toRemove = centerHandles[centerHandles.size() - 2]; //! remove the second-to last point
        removeHandlesFrom(toRemove);
        centerHandles.removeObject(toRemove);
        resized();
    }
}
//=====================================================================
LFOEditorPanel::LFOEditorPanel(lfoArray* array) :
editor(std::make_unique<LFOEditor>(array))
{
    addAndMakeVisible(&addButton);
    addAndMakeVisible(&delButton);
    addAndMakeVisible(&*editor);
    addButton.addListener(this);
    delButton.addListener(this);
}

void LFOEditorPanel::resized()
{
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getHeight() / 8.5f;
    auto buttonBox = fBounds.removeFromTop(dX);
    auto minorTrim = dX / 6.0f;
    addButton.setBounds(buttonBox.removeFromLeft(2 * dX).reduced(minorTrim).toType<int>());
    delButton.setBounds(buttonBox.removeFromLeft(2 * dX).reduced(minorTrim).toType<int>());
    editor->setBounds(fBounds.toType<int>());
}

void LFOEditorPanel::paint(juce::Graphics &g)
{
    
}

void LFOEditorPanel::buttonClicked(juce::Button *b)
{
    if(b == &addButton)
    {
        editor->addPoint();
    }
    if(b == &delButton)
    {
        editor->removePoint();
    }
}
