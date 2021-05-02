/*
  ==============================================================================

    ParameterComponent.h
    Created: 1 May 2021 6:01:51pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "CustomLnFs.h"
#include "ModulationSystem.h"

class SelectorButton : public juce::ShapeButton
{
public:
    SelectorButton(int index) : juce::ShapeButton("ModSelectButton" + juce::String(index),
                                                  UXPalette::modTargetShades[index],
                                                  UXPalette::modTargetShades[index],
                                                  UXPalette::modTargetShades[index]),
    centerColor(UXPalette::modTargetShades[index])
    {
        
    }
    void paintButton(juce::Graphics& g, bool, bool) override;
    juce::Colour centerColor;
};

class CloseButton : public juce::ShapeButton
{
public:
    CloseButton(int index) : juce::ShapeButton("ModDeleteButton" + juce::String(index),
                                               UXPalette::modTargetShades[index],
                                               UXPalette::modTargetShades[index],
                                               UXPalette::modTargetShades[index])
    {
        
    }
    void paintButton(juce::Graphics& g, bool, bool) override;
   
};

class SourceButtonGroup : public juce::Component
{
public:
    SourceButtonGroup(int index) :
    srcIndex(index),
    sButton(index),
    cButton(index)
    {
        addAndMakeVisible(&sButton);
        addAndMakeVisible(&cButton);
    }
    int srcIndex;
    SelectorButton sButton;
    CloseButton cButton;
};

class ParamComponent : public juce::Component, public juce::DragAndDropTarget
{
public:
    ParamComponent(SynthParam* p);
    virtual ~ParamComponent() {}
    SynthParam* linkedParam;
    void itemDropped(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override;
    void itemDragEnter(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override{}
    void itemDragExit(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override{}
    void itemDragMove(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override{}
    bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override
    {
        return true;
    }
    bool shouldDrawDragImageWhenOver() override
    {
        return false;
    }
    void mouseDown(const juce::MouseEvent& e) override;
    virtual void addModSource(ParamComponent* srcComp); //override this to add the specific shape of mod source
    juce::Slider mainSlider;
};

class ParamCompRotary : public ParamComponent, public juce::Button::Listener
{
public:
    class DepthSliderRotary : public juce::Component, juce::Slider::Listener
    {
    public:
        DepthSliderRotary(ModSource* s);
        ~DepthSliderRotary() {dSlider.setLookAndFeel(nullptr);}
        ModSource* src;
        void sliderValueChanged(juce::Slider* s) override
        {
            src->depth = s->getValue();
        }
        void setFront(bool shouldBeInFront)
        {
            setVisible(shouldBeInFront);
            setEnabled(shouldBeInFront);
        }
        juce::Slider dSlider;
        DepthSliderLookAndFeel depthLnF;
    };
    ParamCompRotary(SynthParam* p);
    ~ParamCompRotary() {mainSlider.setLookAndFeel(nullptr); }
    void buttonClicked(juce::Button* b) override;
    void addModSource(ParamComponent* src) override;
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    juce::OwnedArray<DepthSliderRotary> depthSliders;
    ModSystemLookAndFeel LnF;
};
