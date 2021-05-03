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
#define REPAINT_FPS 24

class EnvelopePanel : public juce::Component
{
public:
    class EnvelopeGraph : public juce::AnimatedAppComponent
    {
    public:
        EnvelopeGraph(EnvelopePanel* panel);
        EnvelopePanel* const linkedPanel;
        void update() override
        {
            updateNumbers();
            repaint();
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
    };
    EnvelopePanel(SynthParam* delay,
                  SynthParam* attack,
                  SynthParam* hold,
                  SynthParam* decay,
                  SynthParam* sustain,
                  SynthParam* release);
    void resized() override;
    void paint(juce::Graphics& g) override;
    ParamCompVertical delayComp;
    ParamCompVertical attackComp;
    ParamCompVertical holdComp;
    ParamCompVertical decayComp;
    ParamCompVertical sustainComp;
    ParamCompVertical releaseComp;
    EnvelopeGraph graph;
};



class OscillatorPanel : public juce::Component
{
public:
    OscillatorPanel(SynthParam* lParam, SynthParam* pParam);
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    ParamCompRotary levelComp;
    ParamCompRotary posComp;
};

class SoundSourcePanel : public juce::Component
{
public:
    SoundSourcePanel(SynthParameterGroup* paramGrp, int index);
    void resized() override;
private:
    OscillatorPanel oscPanel;
    EnvelopePanel envPanel;
};

class OctaneEditor : public juce::Component, public juce::DragAndDropContainer
{
public:
    OctaneEditor(SynthParameterGroup* allParams);
    void resized() override;
private:
    juce::OpenGLContext glContext;
    std::vector<juce::Rectangle<int>> oscBoundRects;
    SynthParameterGroup* paramGroup;
    juce::OwnedArray<SoundSourcePanel> oscPanels;
    
};


