/*
  ==============================================================================

    SliderTypes.h
    Created: 7 Mar 2021 12:46:05pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "GlobalColor.h"
#include "CustomLnFs.h"
#include "ModTargetComponent.h"

enum EnvSliderType
{
    DelaySlider = 0,
    AttackSlider,
    HoldSlider,
    DecaySlider,
    SustainSlider,
    ReleaseSlider
};

class EnvelopeLabel : public juce::Label, public juce::Slider::Listener
{
public:
    EnvelopeLabel(juce::Slider* s, std::string suff) : source(s), suffix(suff)
    {
        s->addListener(this);
        auto vcrFont = juce::Font("WW Digital", 8.0f, 0);
        setEditable(true);
        getLookAndFeel().setDefaultSansSerifTypeface(getLookAndFeel().getTypefaceForFont(vcrFont));
        setJustificationType(juce::Justification::centred);
        setMinimumHorizontalScale(0.25);
        juce::String tempText;
        auto valueToDraw = source->getValue();
        auto fullString = juce::String(valueToDraw);
        if(valueToDraw < 100.0f)
            tempText = fullString.substring(0, 2);
        else if(valueToDraw < 1000.0f)
            tempText = fullString.substring(0, 3);
        else
            tempText = fullString.substring(0, 4);
        auto labelText = tempText +  suffix;
        if(suffix == "")
            labelText = tempText;
        setText(labelText, juce::dontSendNotification);
    }
    void sliderValueChanged(juce::Slider* s) override
    {
        juce::String tempText;
        juce::String labelText;
        auto valueToDraw = source->getValue();
        auto fullString = juce::String(valueToDraw);
        if(suffix == "")
        {
            labelText = fullString;
        }
        else
        {
            if(valueToDraw < 100.0f)
                tempText = fullString.substring(0, 3);
            else if(valueToDraw < 1000.0f)
                tempText = fullString.substring(0, 4);
            else
                tempText = fullString.substring(0, 5);
            labelText = tempText +  suffix;
        }
        setText(labelText, juce::dontSendNotification);
    }
    void textWasEdited() override
    {
        auto str = getText(true);
        auto val = str.getDoubleValue();
        source->setValue(val);
    }
    juce::Slider* source;
    std::string suffix;
};

class EnvelopeDial : public ModTargetSlider
{
public:
    EnvelopeDial(juce::DragAndDropContainer* c, EnvSliderType t, int index) : ModTargetSlider(c), type(t), srcIndex(index)
    {
        switch(type)
        {
            case DelaySlider:{
                mTarget.setRange(0.0f, 20000.0f);
                mTarget.setValue(0.0f);
                mTarget.setSkewFactorFromMidPoint(1200.0f);
                mTarget.desc = "delaySlider";
                break;
            }
            case AttackSlider:{
                mTarget.setRange(0.0f, 20000.0f);
                mTarget.setValue(25.0f);
                mTarget.setSkewFactorFromMidPoint(1200.0f);
                mTarget.desc = "attackSlider";
                break;
            }
            case HoldSlider:{
                mTarget.setRange(0.0f, 20000.0f);
                mTarget.setValue(0.0f);
                mTarget.setSkewFactorFromMidPoint(1200.0f);
                mTarget.desc = "holdSlider";
                break;
            }
            case DecaySlider:{
                mTarget.setRange(0.0f, 20000.0f);
                mTarget.setValue(75.0f);
                mTarget.setSkewFactorFromMidPoint(1200.0f);
                mTarget.desc = "decaySlider";
                break;
            }
            case SustainSlider:{
                mTarget.setRange(0.0f, 1.0f);
                mTarget.setValue(0.6f);
                mTarget.desc = "sustainSlider";
                break;
            }
            case ReleaseSlider:{
                mTarget.setRange(0.0f, 20000.0f);
                mTarget.setValue(100.0f);
                mTarget.setSkewFactorFromMidPoint(1200.0f);
                mTarget.desc = "releaseSlider";
                break;
            }
        }
        mTarget.desc += std::to_string(srcIndex);
    }
    EnvSliderType type;
    int srcIndex;
};

class TablePositionSlider : public ModTargetSlider
{
public:
    TablePositionSlider(juce::DragAndDropContainer* c, int index) :
    ModTargetSlider(c, "positionSlider" + std::to_string(index), 0.0f, 1.0f, 0.0f),
    srcIndex(index)
    {
        
    }
private:
    int srcIndex;
};

class OscLevelSlider : public ModTargetSlider
{
public:
    OscLevelSlider(juce::DragAndDropContainer* c, int index) :
    ModTargetSlider(c, "oscLevelSlider" + std::to_string(index), 0.0f, 1.0f, 0.0f),
    srcIndex(index)
    {
        
    }
private:
    int srcIndex;
};


