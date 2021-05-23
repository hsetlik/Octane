/*
  ==============================================================================

    ParameterLabel.h
    Created: 23 May 2021 11:08:49am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "ParameterComponent.h"

class ParamLabel : public juce::Label
{
public:
    ParamLabel(ParamComponent* comp) : linkedComp(comp)
    {
        setEditable(false);
        attachToComponent(linkedComp, false);
        setMinimumHorizontalScale(0.3f);
        //! determine the label text from the param name
        auto rawName = linkedComp->linkedParam->name;
        juce::String labelText;
        if(rawName.contains("Oscillator"))
            labelText = rawName.trimCharactersAtStart("Oscillator");
        else if(rawName.contains("Lfo"))
            labelText = rawName.trimCharactersAtStart("Lfo");
        if(labelText.contains("Unison"))
            labelText = labelText.trimCharactersAtStart("Unison");
        labelText = labelText.dropLastCharacters(1);
        setText(labelText, juce::dontSendNotification);
    }
    ParamComponent* const linkedComp;
    
};
