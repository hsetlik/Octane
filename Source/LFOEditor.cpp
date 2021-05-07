/*
  ==============================================================================

    LFOEditor.cpp
    Created: 6 May 2021 5:37:07pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "LFOEditor.h"


LFOPointHandle::CenterHandle::CenterHandle(Constrainer* c, juce::Component* f, juce::Component* r) :
constrainer(c),
frontHandle(f),
rearHandle(r)
{
    
}

LFOPointHandle::LFOPointHandle(Constrainer* c, fPoint center) :
constrainer(c),
point(center.getX(), center.getY(), false),
centerComp(c, &front, &rear),
front(constrainer),
rear(constrainer)
{
    addAndMakeVisible(&centerComp);
}

LFOPointHandle::LFOPointHandle(Constrainer* c, fPoint center, fPoint f, fPoint r, bool isSplit) :
constrainer(c),
point(center.getX(), center.getY(), true, isSplit),
centerComp(c, &front, &rear),
front(constrainer),
rear(constrainer)
{
    addAndMakeVisible(&centerComp);
    addAndMakeVisible(&front);
    addAndMakeVisible(&rear);
}

LFOEditor::LFOEditor(lfoArray* arr) :
linkedArray(arr)
{
    
}
