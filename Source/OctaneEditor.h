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
    class EnvelopeGraph : public juce::Component, public juce::Timer
    {
    public:
        EnvelopeGraph(EnvelopePanel* panel);
        EnvelopePanel* const linkedPanel;
        void timerCallback() override
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



class OscillatorPanel : public juce::Component
{
public:
    class WaveDisplay : public juce::AnimatedAppComponent
    {
    public:
        WaveDisplay(std::vector<std::vector<float>> d);
        void update() override;
        void paint(juce::Graphics& g) override;
        void createWaves();
        void alterFor3d(juce::Path* p, float index)
        {
            auto numTraces = paths.size();
            auto fBounds = getBounds().toFloat();
            auto dX = fBounds.getWidth() / numTraces / 4;
            auto dY = fBounds.getHeight() / numTraces / 4;
            auto t = juce::AffineTransform::scale(0.55f, 0.55f).followedBy(juce::AffineTransform::shear(0.0f, 0.2f)).followedBy(juce::AffineTransform::translation((dX * index) + (fBounds.getWidth() / 8), -(dY * index * 0.7f) +  (fBounds.getHeight() / 5)));
            p->applyTransform(t);
        }
        void setData(std::vector<std::vector<float>>& d) {data = d; }
    private:
        std::vector<std::vector<float>> data;
        juce::OwnedArray<juce::Path> paths;
    };
    OscillatorPanel(SynthParam* lParam, SynthParam* pParam, std::vector<std::vector<float>> d);
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    ParamCompRotary levelComp;
    ParamCompRotary posComp;
    WaveDisplay graph;
    
};

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

class OctaneEditor : public juce::Component, public juce::DragAndDropContainer
{
public:
    OctaneEditor(SynthParameterGroup* allParams);
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    juce::OpenGLContext glContext;
    std::vector<juce::Rectangle<int>> oscBoundRects;
    SynthParameterGroup* paramGroup;
    juce::OwnedArray<SoundSourcePanel> oscPanels;
    
};


