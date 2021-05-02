/*
  ==============================================================================

    ParameterComponent.cpp
    Created: 1 May 2021 6:01:51pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "ParameterComponent.h"
void SelectorButton::paintButton(juce::Graphics &g, bool, bool)
{
    g.setColour(juce::Colours::black.brighter(0.1f));
    g.fillEllipse(getLocalBounds().toFloat());
    g.setColour(centerColor);
    g.fillEllipse(getLocalBounds().toFloat().reduced(3.5f));
}

void CloseButton::paintButton(juce::Graphics &g, bool, bool)
{
    juce::Path p;
    auto bounds = getLocalBounds().toFloat();
    g.setColour(Color::RGBColor(186, 51, 77));
    g.fillEllipse(bounds);
    g.setColour(Color::RGBColor(112, 32, 51));
    auto centerX = getBounds().toFloat().getWidth() / 2.0f;
    auto centerY = getBounds().toFloat().getHeight() / 2;
    auto rWidth = bounds.getWidth() * 0.2f;
    auto rLength = bounds.getHeight() * 0.8f;
    auto fRadius = rWidth * 0.3f;
    g.saveState();
    p.addRoundedRectangle(centerX - (rWidth / 2.0f), centerY - (rLength / 2.0f), rWidth, rLength, fRadius);
    p.addRoundedRectangle(centerX - (rLength / 2.0f), centerY - (rWidth / 2.0f), rLength, rWidth, fRadius);
    g.addTransform(juce::AffineTransform::rotation(juce::MathConstants<float>::halfPi / 2, centerX, centerY));
    g.fillPath(p);
    g.restoreState();
}

//===============================================================================================================

ParamComponent::ParamComponent(SynthParam* p) : linkedParam(p)
{
    
}

void ParamComponent::itemDropped(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    ParamComponent* sourceComp;
    if((sourceComp = dynamic_cast<ParamComponent*>(dragSourceDetails.sourceComponent.get())))
        addModSource(sourceComp);
}

void ParamComponent::mouseDown(const juce::MouseEvent &e)
{
    juce::DragAndDropContainer::findParentDragContainerFor(this)->startDragging(linkedParam->name, this);
}

//======================================================================================================

ParamCompRotary::ParamCompRotary(SynthParam* p) : ParamComponent(p)
{
    addAndMakeVisible(&mainSlider);
    mainSlider.setSliderStyle(juce::Slider::Rotary);
    mainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
    mainSlider.setRange(linkedParam->min, linkedParam->max);
    mainSlider.setValue(linkedParam->baseValue);
    mainSlider.setLookAndFeel(&LnF);
}

ParamCompRotary::DepthSliderRotary::DepthSliderRotary(ModSource* s) : src(s)
{
    dSlider.addListener(this);
    addAndMakeVisible(&dSlider);
    dSlider.setRange(0.0f, 1.0f);
    dSlider.setValue(0.5f);
    dSlider.setSliderStyle(juce::Slider::Rotary);
    dSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
    dSlider.setLookAndFeel(&depthLnF);
}
//==========================================================================
void ParamCompRotary::SourceButtonsRotary::resized()
{
    auto angle = (juce::MathConstants<float>::pi / 8) * srcIndex;
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getWidth() / 9;
    auto xCenter = fBounds.getWidth() / 2;
    auto yCenter = fBounds.getHeight() / 2;
    sButton.setBounds(4 * dX, 0, dX, dX);
    sButton.setTransform(juce::AffineTransform::rotation(angle, xCenter, yCenter));
    cButton.setBounds(4 * dX, 0, dX, dX);
    cButton.setTransform(juce::AffineTransform::rotation(juce::MathConstants<float>::pi / 4.0f));
}

void ParamCompRotary::SourceButtonsRotary::paint(juce::Graphics &g)
{
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getWidth() / 9;
    auto closeBkgnd = cButton.getLocalBounds().toFloat().expanded(dX);
    auto mainBkgnd = fBounds.withSizeKeepingCentre(5 * dX, 5 * dX);
    auto selBkgnd = sButton.getLocalBounds().toFloat().expanded(dX);
    g.setColour(UXPalette::modTargetShades[srcIndex]);
    g.fillEllipse(closeBkgnd);
    g.fillPath(PathUtility::betweenEllipses(mainBkgnd, closeBkgnd));
    g.fillEllipse(mainBkgnd);
    g.fillPath(PathUtility::betweenEllipses(mainBkgnd, selBkgnd));
    g.fillEllipse(selBkgnd);
}

//=================================================================================

void ParamCompRotary::addModSource(ParamComponent *src)
{
    printf("%s is adding Source: %s\n", linkedParam->name.toRawUTF8(), src->linkedParam->name.toRawUTF8());
    depthSliders.add(new DepthSliderRotary(src->linkedParam->makeSource(0.5f)));
    auto nSlider = depthSliders.getLast();
    addAndMakeVisible(nSlider);
}

void ParamCompRotary::buttonClicked(juce::Button *b)
{
    
}

void ParamCompRotary::resized()
{
    auto fBounds = getBounds().toFloat();
    auto dX = fBounds.getWidth() / 10;
    for(auto group : buttonGroups)
    {
        group->setBounds(fBounds.toType<int>());
    }
    auto innerBounds = fBounds.withSizeKeepingCentre(6 * dX, 6 * dX);
    for(auto slider : depthSliders)
    {
        slider->setBounds(innerBounds.toType<int>());
    }
    innerBounds = fBounds.withSizeKeepingCentre(5 * dX, 5 * dX);
    mainSlider.setBounds(innerBounds.toType<int>());
    mainSlider.toFront(true);
}

void ParamCompRotary::paint(juce::Graphics &g)
{
    
}

