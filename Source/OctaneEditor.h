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
#include "LFOEditor.h"
#include "ParameterLabel.h"
using apvts = juce::AudioProcessorValueTreeState;

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
        float cMin;
        float cMax;
        float exp;
        float difference;
    };
    class EnvLevelMeter : public juce::AnimatedAppComponent
    {
    public:
        EnvLevelMeter(SynthParam* output, SynthParam* lastVoice);
        SynthParam* const levelParam;
        SynthParam* const lastVoiceParam;
        void update() override;
        void paint(juce::Graphics& g) override;
        int lastVoice;
        float level;
    };
    EnvelopePanel(SynthParam* delay,
                  SynthParam* attack,
                  SynthParam* hold,
                  SynthParam* decay,
                  SynthParam* sustain,
                  SynthParam* release,
                  SynthParam* src,
                  SynthParam* lastVoice,
                  apvts* tree);
    void resized() override;
    void paint(juce::Graphics& g) override;
    ParamCompVertical delayComp;
    ParamCompVertical attackComp;
    ParamCompVertical holdComp;
    ParamCompVertical decayComp;
    ParamCompVertical sustainComp;
    ParamCompVertical releaseComp;
    
    ParamCompSource srcComp;
    EnvLevelMeter meter;
    EnvelopeGraph graph;
    apvts* const linkedTree;
    std::unique_ptr<apvts::SliderAttachment> delayAttach;
    std::unique_ptr<apvts::SliderAttachment> attackAttach;
    std::unique_ptr<apvts::SliderAttachment> holdAttach;
    std::unique_ptr<apvts::SliderAttachment> decayAttach;
    std::unique_ptr<apvts::SliderAttachment> sustainAttach;
    std::unique_ptr<apvts::SliderAttachment> releaseAttach;
};
//====================================================================================================================
class ParamPowerButton : public juce::Component, public juce::Button::Listener
{
public:
    ParamPowerButton(SynthParam* p) : linkedParam(p)
    {
        addAndMakeVisible(&button);
        button.addListener(this);
    }
    SynthParam* const linkedParam;
    void buttonClicked(juce::Button* b) override
    {
        if(b->getToggleState())
            linkedParam->setBase(1.0f);
        else
            linkedParam->setBase(0.0f);
        getParentComponent()->repaint();
    }
    void resized() override
    {
        button.setBounds(getLocalBounds());
    }
    PowerButtonCore button;
};
//====================================================================================================================
class UnisonPanel : public juce::Component
{
public:
    UnisonPanel(SynthParam* mode, SynthParam* voices, SynthParam* spread, SynthParam* level);
    void resized() override;
private:
    ParamPowerButton modeButton;
    ParamCompRotary voiceComp;
    ParamCompRotary spreadComp;
    ParamCompRotary levelComp;
    ParamLabel voiceLabel;
    ParamLabel spreadLabel;
    ParamLabel levelLabel;
};
//====================================================================================================================
class LFOPanel : public juce::Component, public juce::Button::Listener
{
public:
    class LevelMeter : public juce::AnimatedAppComponent
    {
    public:
        LevelMeter(SynthParam* output, SynthParam* retrig, SynthParam* lastVoice) :
        outputParam(output),
        retrigParam(retrig),
        lastVoiceParam(lastVoice),
        level(0.0f)
        {
            setFramesPerSecond(REPAINT_FPS);
        }
        SynthParam* const outputParam;
        SynthParam* const retrigParam;
        SynthParam* const lastVoiceParam;
        void update() override
        {
            if(retrigParam->getActual() < 1.0f)
                level = outputParam->getAdjusted(0);
            else
            {
                level = outputParam->getAdjusted((int)lastVoiceParam->getActual());
            }
            repaint();
        }
        void paint(juce::Graphics& g) override
        {
            auto fBounds = getLocalBounds().toFloat();
            auto height = fBounds.getHeight();
            auto meterTop = height - (height * level);
            g.setColour(UXPalette::lightGray);
            g.fillRect(fBounds);
            g.setColour(UXPalette::highlight);
            g.fillRect(0.0f, meterTop, fBounds.getWidth(), height * level);
        }
    private:
        float level;
    };
    class RetrigButton : public juce::ShapeButton
    {
    public:
        RetrigButton();
        void paintButton(juce::Graphics& g, bool mouseOver, bool isMouseDown) override;
    };
    LFOPanel(SynthParam* rate, SynthParam* retrig, SynthParam* src, SynthParam* power, SynthParam* lastVoice, lfoArray* array, apvts* tree, OctaneUpdater* updater, int index);
    void buttonClicked(juce::Button* b) override
    {
        float newValue = (b->getToggleState()) ? 1.0f : 0.0f;
        retrigParam->setBase(newValue);
    }
    void togglePower() {pButton.button.triggerClick();}
    void resized() override;
    void paint(juce::Graphics& g) override;
    ParamCompRotary rateComp;
    ParamCompSource outputComp;
    LFOEditorPanel editor;
    RetrigButton rButton;
    LevelMeter meter;
    ParamPowerButton pButton;
    SynthParam* const retrigParam;
    lfoArray* const linkedArray;
    apvts* const linkedTree;
    std::unique_ptr<apvts::SliderAttachment> rateAttach;
    const int lfoIndex;
};
//====================================================================================================================
class FilterPanel :
public juce::Component,
public juce::ComboBox::Listener
{
public:
    FilterPanel(SynthParam* cutoff, SynthParam* resonance, SynthParam* wetDry, OctaneUpdater* updater, apvts* tree);
    SynthParam* const cutoffParam;
    SynthParam* const resonanceParam;
    OctaneUpdater* const linkedUpdater;
    apvts* const linkedTree;
    void comboBoxChanged(juce::ComboBox* b) override;
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    ParamCompRotary cutoffComp;
    ParamCompRotary resonanceComp;
    ParamCompRotary wetDryComp;
    
    juce::ComboBox typeBox;
};

//====================================================================================================================
class WaveSelector :
public juce::Component,
public juce::Button::Listener,
public juce::AsyncUpdater
{
public:
    WaveSelector(SynthParam* pParam, OctaneUpdater* updater, int index);
    void resized() override;
    void buttonClicked(juce::Button* b) override;
    SynthParam* const positionParam;
    OctaneUpdater* const linkedUpdater;
    const int oscIndex;
    void replaceFromIndex(int index);
    void handleAsyncUpdate() override;
private:
    juce::File selectedFile;
    std::unique_ptr<WaveGraphOpenGL> display;
    juce::Array<juce::File>* const waveFiles;
    bool needsToUpdate;
public:
    OctaneButtons::Text textButton;
    NextButton nextButton;
    LastButton lastButton;
    int selectedIndex;
    
};
//====================================================================================================================
class OscillatorPanel : public juce::Component
{
public:
    OscillatorPanel(SynthParam* lParam, SynthParam* pParam, SynthParam* panParam, SynthParam* powerParam, OctaneUpdater* updater, apvts* tree, int index);
    void resized() override;
    void paint(juce::Graphics& g) override;
    void togglePower() {powerComp.button.triggerClick();}
    bool powerState()
    {
        return powerComp.button.getToggleState();
    }
private:
    ParamCompRotary levelComp;
    ParamCompRotary posComp;
    ParamCompRotary panComp;
    ParamLabel levelLabel;
    ParamLabel posLabel;
    ParamLabel panLabel;
    ParamPowerButton powerComp;
    std::unique_ptr<apvts::SliderAttachment> levelAttach;
    std::unique_ptr<apvts::SliderAttachment> posAttach;
    std::unique_ptr<apvts::SliderAttachment> panAttach;
    WaveSelector display;
    apvts* const linkedTree;
};

//====================================================================================================================
class SoundSourcePanel : public juce::Component
{
public:
    SoundSourcePanel(SynthParameterGroup* paramGrp, int index, apvts* tree, OctaneUpdater* updater);
    void resized() override;
    void paint(juce::Graphics &g) override;
    void togglePower() {oscPanel.togglePower();}
private:
    OscillatorPanel oscPanel;
    UnisonPanel uPanel;
    EnvelopePanel ampEnvPanel;
    EnvelopePanel modEnvPanel;
    apvts* const linkedTree;
    OctaneUpdater* const linkedUpdater;
};
//================================================================================
class MacroPanel : public juce::Component
{
public:
    MacroPanel(SynthParam* pitch, SynthParam* mod, SynthParam* key);
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    ParamCompSource pitchComp;
    ParamCompSource modComp;
    ParamCompSource keyComp;
    
};
//================================================================================

class OctaneEditor : public juce::Component, public juce::DragAndDropContainer
{
public:
    OctaneEditor(SynthParameterGroup* allParams, apvts* tree, OctaneUpdater* update);
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    std::vector<juce::Rectangle<int>> oscBoundRects;
    SynthParameterGroup* const paramGroup;
    apvts* linkedTree;
    OctaneUpdater* const linkedUpdater;
    juce::OwnedArray<SoundSourcePanel> oscPanels;
    juce::OwnedArray<LFOPanel> lfoPanels;
    FilterPanel filterPanel;
    MacroPanel macroPanel;
    
};


