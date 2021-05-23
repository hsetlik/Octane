/*
  ==============================================================================

    ParameterLabel.h
    Created: 23 May 2021 11:08:49am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "ParameterComponent.h"

enum LabelJustification
{
    AboveCenter,
    BelowCenter
};
class ParamLabel : public juce::Component, public juce::ComponentListener
{
public:
    ParamLabel(ParamComponent* comp, LabelJustification just=AboveCenter) :
    linkedComp(comp),
    justif(just),
    labelWidth(0),
    fontSize(1.0f)
    {
        linkedComp->addComponentListener(this);
        //! determine the label text from the param name
        auto rawName = linkedComp->linkedParam->name;
        if(rawName.contains("Oscillator"))
            labelText = rawName.trimCharactersAtStart("Oscillator");
        else if(rawName.contains("Lfo"))
            labelText = rawName.trimCharactersAtStart("Lfo");
        if(labelText.contains("Unison"))
            labelText = labelText.trimCharactersAtStart("Unison");
        labelText = labelText.dropLastCharacters(1);
    }
    ParamComponent* const linkedComp;
    void resized() override;
    void paint(juce::Graphics& g) override;
    void componentMovedOrResized(juce::Component& comp, bool moved, bool resized) override;
private:
    LabelJustification justif;
    juce::String labelText;
    int labelWidth;
    float fontSize;
    
};
