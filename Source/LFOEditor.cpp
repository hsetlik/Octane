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


LFOEditor::LFOEditor(lfoArray* arr) :
startPoint(0.0f, 0.0f, false),
endPoint(1.0f, 0.0f, false),
linkedArray(arr)
{
   
}

void LFOEditor::timerCallback()
{
    
}

void LFOEditor::resized()
{
    
}

void LFOEditor::paint(juce::Graphics &g)
{
    g.fillAll(UXPalette::darkBkgnd);
    
}
void LFOEditor::connectPoints(LFOPoint* first, LFOPoint* second)
{
    
}

