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



//=================================================================

LFOEditor::LFOEditor(lfoArray* arr) :
startPoint(0.0f, 0.0f, false),
endPoint(1.0f, 0.0f, false),
linkedArray(arr)
{
    addAndMakeVisible(pointHandles.add(new CenterHandle(nullptr, nullptr, this, 0.05f, 0.95f)));
    addAndMakeVisible(pointHandles.add(new CenterHandle(nullptr, nullptr, this, 0.5f, 0.05f)));
    addAndMakeVisible(pointHandles.add(new CenterHandle(nullptr, nullptr, this, 0.95f, 0.95f)));
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

