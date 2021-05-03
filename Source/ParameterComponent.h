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
#include "GraphicsUtility.h"
#include "StlUtil.h"

class SelectorButton : public juce::ShapeButton
{
public:
    SelectorButton(int index) : juce::ShapeButton("ModSelectButton" + juce::String(index),
                                                  UXPalette::modTargetShades[index],
                                                  UXPalette::modTargetShades[index],
                                                  UXPalette::modTargetShades[index]),
    centerColor(UXPalette::modTargetShades[index]),
    groupIndex(index)
    {
    
    }
    void paintButton(juce::Graphics& g, bool, bool) override;
    juce::Colour centerColor;
    int groupIndex;
};

class CloseButton : public juce::ShapeButton
{
public:
    CloseButton(int index) : juce::ShapeButton("ModDeleteButton" + juce::String(index),
                                               UXPalette::modTargetShades[index],
                                               UXPalette::modTargetShades[index],
                                               UXPalette::modTargetShades[index]),
    groupIndex(index)
    {
        
    }
    void paintButton(juce::Graphics& g, bool, bool) override;
    int groupIndex;
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
        setInterceptsMouseClicks(true, true);
    }
    void attach(juce::Button::Listener* list)
    {
        sButton.addListener(list);
        cButton.addListener(list);
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
    SynthParam* const linkedParam;
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
    virtual void addModSource(ParamComponent* srcComp) {} //override this to add the specific shape of mod source
    void removeModSource(ParamComponent* toRemove)
    {
        int idx = 0;
        for(auto src : addedSources)
        {
            if(toRemove == src)
            {
                addedSources.erase(addedSources.begin() + idx);
                return;
            }
            ++idx;
        }
    }
    
    juce::Slider mainSlider;
    std::vector<ParamComponent*> addedSources;
};

class ParamCompRotary : public ParamComponent, public juce::Button::Listener, public juce::Slider::Listener
{
public:
    class DepthSliderRotary : public juce::Component, juce::Slider::Listener
    {
    public:
        DepthSliderRotary(ModSource* s, ParamComponent* sComp);
        ~DepthSliderRotary() {dSlider.setLookAndFeel(nullptr);}
        ModSource* const src;
        ParamComponent* const srcComp;
        void sliderValueChanged(juce::Slider* s) override
        {
            src->depth = s->getValue();
        }
        void setFront(bool shouldBeInFront)
        {
            setVisible(shouldBeInFront);
            setEnabled(shouldBeInFront);
        }
        void resized() override
        {
            dSlider.setBounds(getLocalBounds());
        }
        juce::Slider dSlider;
        DepthSliderLookAndFeel depthLnF;
    };
    class SourceButtonsRotary : public SourceButtonGroup
    {
    public:
        SourceButtonsRotary(int idx) : SourceButtonGroup(idx)
        {
            
        }
        void resized() override;
        void paint(juce::Graphics& g) override;
    };
    ParamCompRotary(SynthParam* p);
    ~ParamCompRotary() {mainSlider.setLookAndFeel(nullptr); }
    void buttonClicked(juce::Button* b) override;
    void sliderValueChanged(juce::Slider* s) override;
    void addModSource(ParamComponent* src) override;
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    void resetIndeces()
    {
        for(int i = 0; i < buttonGroups.size(); ++i)
        {
            buttonGroups[i]->srcIndex = i;
        }
    }
    juce::OwnedArray<DepthSliderRotary> depthSliders;
    juce::OwnedArray<SourceButtonsRotary> buttonGroups;
    SourceButtonsRotary* currentButttons;
    DepthSliderRotary* currentDepthSlider;
    ModSystemLookAndFeel LnF;
    int selectedIndex;
};

//=========================================================================

class ParamCompVertical : public ParamComponent, public juce::Button::Listener, public juce::Slider::Listener
{
public:
    class DepthSliderVertical : public juce::Component, public juce::Slider::Listener
    {
    public:
        DepthSliderVertical(ModSource* s, ParamComponent* sComp);
        ~DepthSliderVertical() {dSlider.setLookAndFeel(nullptr); }
        ModSource* const src;
        ParamComponent* const srcComp;
        void sliderValueChanged(juce::Slider* s) override
        {
            src->depth = s->getValue();
        }
        void setFront(bool shouldBeInFront)
        {
            setVisible(shouldBeInFront);
            setEnabled(shouldBeInFront);
        }
        void resized() override
        {
            dSlider.setBounds(getLocalBounds());
        }
        juce::Slider dSlider;
        DepthSliderLookAndFeel depthLnF;
    };
    class SourceButtonsVertical : public SourceButtonGroup
    {
    public:
        SourceButtonsVertical(int idx) : SourceButtonGroup(idx)
        {
            
        }
        void resized() override;
        void paint(juce::Graphics& g) override;
    };
    ParamCompVertical(SynthParam* p);
    ~ParamCompVertical() {mainSlider.setLookAndFeel(nullptr); }
    void buttonClicked(juce::Button* b) override;
    void sliderValueChanged(juce::Slider* s) override
    {
        linkedParam->setDierct(s->getValue());
    }
    void addModSource(ParamComponent* src) override;
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    void resetIndeces()
    {
        for(int i = 0; i < buttonGroups.size(); ++i)
        {
            buttonGroups[i]->srcIndex = i;
        }
    }
    juce::OwnedArray<DepthSliderVertical> depthSliders;
    juce::OwnedArray<SourceButtonsVertical> buttonGroups;
    SourceButtonsVertical* currentButttons;
    DepthSliderVertical* currentDepthSlider;
    ModSystemLookAndFeel LnF;
    int selectedIndex;
};
