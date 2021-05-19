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

ParamComponent::ParamComponent(SynthParam* p) : linkedParam(p), source(false)
{
    setInterceptsMouseClicks(true, true);
}

void ParamComponent::itemDropped(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    ParamComponent* sourceComp;
    if((sourceComp = dynamic_cast<ParamComponent*>(dragSourceDetails.sourceComponent.get())))
    {
        if(!StlUtil::existsIn(addedSources, sourceComp) &&(sourceComp != this) && (sourceComp->isSource()))
        {
            addedSources.push_back(sourceComp);
            addModSource(sourceComp);
        }
    }
        
}

void ParamComponent::mouseDown(const juce::MouseEvent &e)
{
    if(isSource())
        juce::DragAndDropContainer::findParentDragContainerFor(this)->startDragging(linkedParam->name, this);
}


//======================================================================================================

ParamCompRotary::ParamCompRotary(SynthParam* p) :
ParamComponent(p),
currentButttons(nullptr),
currentDepthSlider(nullptr), //! these get initialized to \c nullptr because there are no mod sources yet
selectedIndex(0)
{
    addAndMakeVisible(&mainSlider);
    mainSlider.setSliderStyle(juce::Slider::Rotary);
    mainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
    mainSlider.setRange(linkedParam->getMin(), linkedParam->getMax());
    mainSlider.setValue(linkedParam->getBaseValue());
    mainSlider.setLookAndFeel(&LnF);
    mainSlider.addListener(this);
}

void ParamCompRotary::sliderValueChanged(juce::Slider *s)
{
    linkedParam->setBase((float)s->getValue());
}


ParamCompRotary::DepthSliderRotary::DepthSliderRotary(ModSource* s, ParamComponent* sComp) : src(s), srcComp(sComp)
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
    auto angle = (juce::MathConstants<float>::twoPi / 8) * srcIndex;
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getWidth() / 9;
    auto xCenter = fBounds.getWidth() / 2;
    auto yCenter = fBounds.getHeight() / 2;
    sButton.setBounds(5 * dX, dX / 2, dX, dX);
    sButton.setTransform(juce::AffineTransform::rotation(angle, xCenter, yCenter));
    cButton.setBounds(5 * dX, dX / 2, dX, dX);
    cButton.setTransform(juce::AffineTransform::rotation(juce::MathConstants<float>::pi, xCenter, yCenter));
}

void ParamCompRotary::SourceButtonsRotary::paint(juce::Graphics &g)
{
    auto fBounds = getLocalBounds().toFloat();
    auto center = juce::Point<float>(fBounds.getWidth() / 2.0f, fBounds.getHeight() / 2.0f);
    auto angle = (juce::MathConstants<float>::twoPi / 8) * srcIndex;
    auto dX = fBounds.getWidth() / 9;
    auto closeBkgnd = cButton.getBounds().toFloat().expanded(dX / 3);
    closeBkgnd = closeBkgnd.transformedBy(juce::AffineTransform::rotation(juce::MathConstants<float>::pi, center.getX(), center.getY()));
    auto mainBkgnd = fBounds.withSizeKeepingCentre(6 * dX, 6 * dX);
    auto selBkgnd = sButton.getBounds().toFloat().expanded(dX / 3);
    selBkgnd = selBkgnd.transformedBy(juce::AffineTransform::rotation(angle, center.getX(), center.getY()));
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
    auto mSource = src->linkedParam->makeSource(0.5f);
    linkedParam->addSource(mSource);
    depthSliders.add(new DepthSliderRotary(mSource, src));
    buttonGroups.add(new SourceButtonsRotary(depthSliders.size() - 1));
    auto nSlider = depthSliders.getLast();
    auto nButtons = buttonGroups.getLast();
    addAndMakeVisible(nSlider);
    addAndMakeVisible(nButtons);
    nButtons->cButton.addListener(this);
    nButtons->sButton.addListener(this);
    currentButttons = nButtons;
    currentDepthSlider = nSlider;
    resized();
}

void ParamCompRotary::buttonClicked(juce::Button *b)
{
    CloseButton* cButton;
    SelectorButton* sButton;
    if((cButton = dynamic_cast<CloseButton*>(b)))
    {
        auto idx = cButton->groupIndex;
        if(buttonGroups.size() > 1)
        {
            if(idx > 0)
            {
                currentButttons = buttonGroups[idx - 1];
                currentDepthSlider = depthSliders[idx - 1];
            }
            else if(idx == 0)
            {
                currentButttons = buttonGroups[idx + 1];
                currentDepthSlider = depthSliders[idx + 1];
            }
        }
        else
        {
            //! set these back to \c nullptr when all sources are removed
            currentButttons = nullptr;
            currentDepthSlider = nullptr;
        }
        removeModSource(depthSliders[idx]->srcComp);
        buttonGroups.remove(idx);
        depthSliders.remove(idx);
        resetIndeces();
    }
    else if((sButton = dynamic_cast<SelectorButton*>(b)))
    {
        auto idx = sButton->groupIndex;
        currentButttons = buttonGroups[idx];
        currentDepthSlider = depthSliders[idx];
    }
    resized();
}

void ParamCompRotary::resized()
{
    resetIndeces();
    auto fBounds = getLocalBounds().toFloat();
    //printf("Knob at : %f, %f, %f, %f\n", fBounds.getX(), fBounds.getY(), fBounds.getWidth(), fBounds.getHeight());
    auto dX = fBounds.getWidth() / 10;
    for(auto group : buttonGroups)
    {
        group->setBounds(fBounds.toType<int>().reduced(dX * 0.5f));
    }
    
    for(auto slider : depthSliders)
    {
        slider->setBounds(fBounds.toType<int>().reduced(2 * dX));
        slider->repaint();
    }
    auto innerBounds = fBounds.toType<int>().reduced(3 * dX);
    if(currentDepthSlider != nullptr)
    {
        currentButttons->toFront(true);
        currentDepthSlider->toFront(true);
        currentButttons->cButton.setVisible(true);
        currentButttons->cButton.setEnabled(true);
        
        for(auto grp : buttonGroups)
        {
            if(grp != currentButttons)
            {
                grp->cButton.setVisible(false);
                grp->cButton.setEnabled(false);
            }
        }
    }
    mainSlider.setBounds(innerBounds.toType<int>());
    mainSlider.toFront(true);
    repaint();
}

void ParamCompRotary::paint(juce::Graphics &g)
{
    auto fBounds = getLocalBounds().toFloat();
    g.setColour(linkedParam->getColor());
    g.fillEllipse(fBounds);
}

//=========================================================================

ParamCompVertical::DepthSliderVertical::DepthSliderVertical(ModSource* s, ParamComponent* sComp) :
src(s),
srcComp(sComp)
{
    dSlider.addListener(this);
    addAndMakeVisible(&dSlider);
    dSlider.setRange(0.0f, 1.0f);
    dSlider.setValue(0.5f);
    dSlider.setSliderStyle(juce::Slider::LinearVertical);
    dSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
    dSlider.setLookAndFeel(&depthLnF);
}

//==========================================================================

void ParamCompVertical::SourceButtonsVertical::resized()
{
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getWidth() / 3.0f;
    auto dY = fBounds.getHeight() / 8.0f;
    if(dX > dY)
        dX = dY;
    auto closeBounds = juce::Rectangle<int>(0, (int)7 * dY, dX, dX);
    cButton.setBounds(closeBounds.reduced(dX / 8));
    auto selBounds = juce::Rectangle<int>(2 * dX, srcIndex * dX, dX, dX);
    sButton.setBounds(selBounds.reduced(dX / 8));
}

void ParamCompVertical::SourceButtonsVertical::paint(juce::Graphics &g)
{
    
}

//==========================================================================

ParamCompVertical::ParamCompVertical(SynthParam* p) :
ParamComponent(p),
currentButttons(nullptr),
currentDepthSlider(nullptr), //! these get initialized to \c nullptr because there are no mod sources yet
selectedIndex(0)
{
    addAndMakeVisible(&mainSlider);
    mainSlider.addListener(this);
    mainSlider.setSliderStyle(juce::Slider::LinearVertical);
    mainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
    mainSlider.setRange(linkedParam->getMin(), linkedParam->getMax());
    mainSlider.setValue(linkedParam->getBaseValue());
    mainSlider.setLookAndFeel(&LnF);
}

void ParamCompVertical::buttonClicked(juce::Button *b)
{
    CloseButton* cButton;
    SelectorButton* sButton;
    if((cButton = dynamic_cast<CloseButton*>(b)))
    {
        auto idx = cButton->groupIndex;
        if(buttonGroups.size() > 1)
        {
            if(idx > 0)
            {
                currentButttons = buttonGroups[idx - 1];
                currentDepthSlider = depthSliders[idx - 1];
            }
            else if(idx == 0)
            {
                currentButttons = buttonGroups[idx + 1];
                currentDepthSlider = depthSliders[idx + 1];
            }
        }
        else
        {
            //! set these back to \c nullptr when all sources are removed
            currentButttons = nullptr;
            currentDepthSlider = nullptr;
        }
        removeModSource(depthSliders[idx]->srcComp);
        buttonGroups.remove(idx);
        depthSliders.remove(idx);
        resetIndeces();
    }
    else if((sButton = dynamic_cast<SelectorButton*>(b)))
    {
        auto idx = sButton->groupIndex;
        currentButttons = buttonGroups[idx];
        currentDepthSlider = depthSliders[idx];
    }
    resized();
}

void ParamCompVertical::addModSource(ParamComponent *src)
{
    printf("%s is adding Source: %s\n", linkedParam->name.toRawUTF8(), src->linkedParam->name.toRawUTF8());
    depthSliders.add(new DepthSliderVertical(src->linkedParam->makeSource(0.5f), src));
    buttonGroups.add(new SourceButtonsVertical(depthSliders.size() - 1));
    auto nSlider = depthSliders.getLast();
    auto nButtons = buttonGroups.getLast();
    addAndMakeVisible(nSlider);
    addAndMakeVisible(nButtons);
    nButtons->cButton.addListener(this);
    nButtons->sButton.addListener(this);
    currentButttons = nButtons;
    currentDepthSlider = nSlider;
    resized();
}

void ParamCompVertical::resized()
{
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getWidth() / 10;
    auto childBounds = fBounds.reduced(dX);
    auto third = childBounds.getWidth() / 4.0f;
    auto depthSliderBounds = juce::Rectangle<float>(childBounds.getX() + (2 * third),
                                                    childBounds.getY(),
                                                    third,
                                                    childBounds.getHeight());
    auto mainSliderBounds = juce::Rectangle<float>(childBounds.getX() + third,
                                                    childBounds.getY(),
                                                    third,
                                                    childBounds.getHeight());
    for(auto grp : buttonGroups)
        grp->setBounds(childBounds.toType<int>());
    for(auto d : depthSliders)
        d->setBounds(depthSliderBounds.toType<int>());
    mainSlider.setBounds(mainSliderBounds.toType<int>());
    if(currentDepthSlider != nullptr)
    {
        currentButttons->toFront(true);
        currentDepthSlider->toFront(true);
        currentButttons->cButton.setVisible(true);
        currentButttons->cButton.setEnabled(true);
        
        for(auto grp : buttonGroups)
        {
            if(grp != currentButttons)
            {
                grp->cButton.setVisible(false);
                grp->cButton.setEnabled(false);
            }
        }
    }
    mainSlider.toFront(true);
    repaint();
}

void ParamCompVertical::paint(juce::Graphics &g)
{
    //g.fillAll(juce::Colours::white);
}
