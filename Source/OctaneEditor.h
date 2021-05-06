/*
  ==============================================================================

    OctaneEditor.h
    Created: 1 May 2021 6:02:07pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "ParameterComponent.h"
#include "DAHDSR.h"
#include "WaveDisplay.h"
#define REPAINT_FPS 24

class EnvelopePanel : public juce::Component
{
public:
    class EnvelopeGraph : public juce::Component, public juce::Timer
    {
    public:
        EnvelopeGraph(EnvelopePanel* panel);
        EnvelopePanel* const linkedPanel;
        void timerCallback() override
        {
            repaint();
        }
        float onCurve(float input) //! input is between 0 and 1
        {
            exp = input / log(input);
            difference = 1.0f - pow(juce::MathConstants<float>::euler, exp);
            return input - difference;
        }
        void toCurve()
        {
            //! put all the values onto a curve and recalculate lengthMs
            lengthMs = 0.0f;
            delayEnd = onCurve(delayEnd);
            lengthMs += delayEnd;
            attackEnd = onCurve(attackEnd);
            lengthMs += attackEnd;
            holdEnd = onCurve(holdEnd);
            lengthMs += holdEnd;
            decayEnd = onCurve(decayEnd);
            lengthMs += decayEnd;
            sustainEnd = onCurve(sustainEnd);
            lengthMs += sustainEnd;
            releaseEnd = onCurve(releaseEnd);
            lengthMs += releaseEnd;
            //!  rescale everything back to 0-1
            delayEnd /= lengthMs;
            attackEnd /= lengthMs;
            holdEnd /= lengthMs;
            decayEnd /= lengthMs;
            sustainEnd /= lengthMs;
            releaseEnd /= lengthMs;
        }
        void updateNumbers();
        void paint(juce::Graphics& g) override;
    private:
        double lengthMs;
        float delayEnd;
        float attackEnd;
        float holdEnd;
        float decayEnd;
        float sustainEnd;
        float releaseEnd;
        float fSustain;
        
        float exp;
        float difference;
    };
    EnvelopePanel(SynthParam* delay,
                  SynthParam* attack,
                  SynthParam* hold,
                  SynthParam* decay,
                  SynthParam* sustain,
                  SynthParam* release,
                  SynthParam* src);
    void resized() override;
    void paint(juce::Graphics& g) override;
    ParamCompVertical delayComp;
    ParamCompVertical attackComp;
    ParamCompVertical holdComp;
    ParamCompVertical decayComp;
    ParamCompVertical sustainComp;
    ParamCompVertical releaseComp;
    
    ParamCompSource srcComp;
    
    EnvelopeGraph graph;
};

//====================================================================================================================
class LFOPanel : public juce::Component, public juce::Button::Listener
{
public:
    class RetrigButton : public juce::ShapeButton
    {
    public:
        RetrigButton();
        void paintButton(juce::Graphics& g, bool mouseOver, bool isMouseDown) override;
    };
    LFOPanel(SynthParam* rate, SynthParam* retrig, SynthParam* src, lfoArray* array);
    void buttonClicked(juce::Button* b) override
    {
        float newValue = (b->getToggleState()) ? 1.0f : 0.0f;
        retrigParam->setBase(newValue);
    }
    void resized() override;
    void paint(juce::Graphics& g) override;
    ParamCompRotary rateComp;
    ParamCompSource outputComp;
    RetrigButton rButton;
    SynthParam* const retrigParam;
    lfoArray* const linkedArray;
};

//====================================================================================================================

class OscillatorPanel : public juce::Component
{
public:
    OscillatorPanel(SynthParam* lParam, SynthParam* pParam, std::vector<std::vector<float>> graphData);
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    ParamCompRotary levelComp;
    ParamCompRotary posComp;
    std::unique_ptr<WaveGraphOpenGL> display;
    
};

//==============================================================================

class SoundSourcePanel : public juce::Component
{
public:
    SoundSourcePanel(SynthParameterGroup* paramGrp, int index);
    void resized() override;
private:
    OscillatorPanel oscPanel;
    EnvelopePanel ampEnvPanel;
    EnvelopePanel modEnvPanel;
};

//================================================================================

class OctaneEditor : public juce::Component, public juce::DragAndDropContainer
{
public:
    OctaneEditor(SynthParameterGroup* allParams);
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    std::vector<juce::Rectangle<int>> oscBoundRects;
    SynthParameterGroup* paramGroup;
    juce::OwnedArray<SoundSourcePanel> oscPanels;
    juce::OwnedArray<LFOPanel> lfoPanels;
    
};


