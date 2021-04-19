/*
  ==============================================================================

    ModTargetComponent.cpp
    Created: 2 Mar 2021 6:03:45pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "ModTargetComponent.h"



SourceButtonGroup::SourceButtonGroup(ModSourceComponent* s, int srcIndex, juce::Button::Listener* l) : sourceComp(s), sourceIndex(srcIndex), selButton(s->getColor(), srcIndex), closeButton(srcIndex)
{
    addAndMakeVisible(&closeButton);
    addAndMakeVisible(&selButton);
    
    closeButton.addListener(l);
    selButton.addListener(l);
    
    background = Color::RGBColor(38, 48, 55);
    setInterceptsMouseClicks(false, true);
}

void SelectorButton::paintButton(juce::Graphics &g, bool, bool)
{
    g.setColour(juce::Colours::black.brighter(0.1f));
    g.fillEllipse(getLocalBounds().toFloat());
    g.setColour(centerColor);
    g.fillEllipse(getLocalBounds().toFloat().reduced(3.5f));
}

void RemoveButton::paintButton(juce::Graphics &g, bool, bool)
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

void SourceButtonGroup::resized()
{
    //reserve the middle two thirds of the component for the drag/drop target
    //target dimensions = 6n, 1.5n on either side
    auto n = getBounds().toFloat().getWidth() /  9.0f;
    closeButton.setBounds(4.5f * n, 0.0f, 1.2f * n, 1.2f * n);
    //determine the position of selector based on number of sources
    //distance from center of target to Center  selButton is always (1.5 * n) + n / 2 = 2 * n
    auto increment = juce::MathConstants<float>::twoPi / 10;
    auto angle = (sourceIndex) * increment;
    auto fortyFive = increment * 1.5f;
    //center of the component
    auto xCenter = 4.5f * n;
    auto yCenter = 4.5f * n;
    selButton.setBounds(3.4f * n, 7.7f * n, 1.5f * n, 1.5f * n);
    closeButton.setTransform(juce::AffineTransform::rotation(fortyFive, xCenter, yCenter));
    selButton.setTransform(juce::AffineTransform::rotation(angle, xCenter, yCenter));
}
 void SourceButtonGroup::paint(juce::Graphics &g)
{
    juce::Path center;
    g.setColour(UXPalette::modTargetShades[sourceIndex]);
    auto n = getBounds().toFloat().getWidth() /  9.0f;
    center.addEllipse(selButton.getBounds().toFloat().expanded(0.1f * n));
    auto selBounds = selButton.getBounds().toFloat().expanded(0.1f * n);
    center.startNewSubPath(selBounds.getX(), selBounds.getY() + (selBounds.getHeight() / 2));
    auto targetBounds = juce::Rectangle<float> {1.75f * n, 1.75f * n, 5.5f * n, 5.5f * n};
    juce::Path middle;
    middle.addEllipse(targetBounds.expanded(0.2f * n));
    g.fillPath(middle);
    center.lineTo(targetBounds.getX() + n, 4.5f * n);
    center.lineTo(targetBounds.getRight() - n, 4.5f * n);
    center.lineTo(selBounds.getRight(), selBounds.getY() + (selBounds.getHeight() / 2.0f));
    center.closeSubPath();
    auto increment = juce::MathConstants<float>::twoPi / 10;
    auto angle = (sourceIndex) * increment;
    center.applyTransform(juce::AffineTransform::rotation(angle, n * 4.5f, n * 4.5f));
    g.fillPath(center);
    juce::Path close;
    auto closeBounds = closeButton.getBounds().toFloat().expanded(0.2f * n);
    close.addEllipse(closeBounds);
    close.startNewSubPath(closeBounds.getX(), closeBounds.getY() + (closeBounds.getHeight() / 2.0f));
    close.lineTo(closeBounds.getRight(), closeBounds.getY() + (closeBounds.getHeight() / 2.0f));
    close.lineTo(targetBounds.getRight() - n, 4.5f * n);
    close.lineTo(targetBounds.getX() + n, 4.5f * n);
    close.closeSubPath();
    close.applyTransform(juce::AffineTransform::rotation(increment * 1.5f, n * 4.5f, n * 4.5f));
    g.fillPath(close);
}

ModTargetComponent::ModTargetComponent(juce::DragAndDropContainer* c) : numSources(0), mTarget(c, this), container(c)
{
    setInterceptsMouseClicks(true, true);
    selectedGroup = nullptr;
    selectedSlider = nullptr;
    addAndMakeVisible(&mTarget);
    //addMouseListener(this, true);
    mTarget.toFront(true);
    targetColors.add(Color::monochromeFrom(Color::RGBColor(169, 179, 193)));
}

void ModTargetComponent::buttonClicked(juce::Button *b)
{
    SelectorButton* sel;
    RemoveButton* rem;
    if((sel = dynamic_cast<SelectorButton*>(b)))
    {
        auto src = dynamic_cast<SourceButtonGroup*>(sel->getParentComponent());
        selectedGroup = src;
        selectedSlider = sliders[sources.indexOf(src)];
        printf("Group %d selected\n", sources.indexOf(selectedGroup));
    }
    else if ((rem = dynamic_cast<RemoveButton*>(b)))
    {
        auto src = dynamic_cast<SourceButtonGroup*>(rem->getParentComponent());
            if(selectedGroup == src)
            {
                if(sources.size() > 0)
                {
                    selectedGroup = sources.getLast();
                    selectedSlider = sliders[sources.indexOf(selectedGroup)];
                }
                else
                {
                    selectedGroup = nullptr;
                    selectedSlider = nullptr;
                }
            }
            auto index = sources.indexOf(src);
            sliders.remove(index);
            sources.removeObject(src);
            --numSources;
            int ind = 1;
            for(auto i : sources)
            {
                i->setIndex(ind);
                ++ind;
            }
            repaint();
    }
    else
    {
        if(sources.size() == 0) //don't need to check for duplicates if there are 0 sources
        {
            ++numSources;
            sources.add(new SourceButtonGroup(mTarget.getNewSource(), numSources, this));
            sliders.add(new DepthSlider(mTarget.getNewSource()));
            addAndMakeVisible(sources.getLast());
            addAndMakeVisible(sliders.getLast());
        }
        else //check for duplicates
        {
            bool added = false;
            for(auto i : sources)
                if(i->getId() == mTarget.getNewSource()->getId())
                    added = true;
            if(!added)
            {
                ++numSources;
                sources.add(new SourceButtonGroup(mTarget.getNewSource(), numSources, this));
                sliders.add(new DepthSlider(mTarget.getNewSource()));
                addAndMakeVisible(sources.getLast());
                addAndMakeVisible(sliders.getLast());
            }
        }
    }
    resized();
}

void ModTargetComponent::resized()
{
    bool validSelected = false;
    if(sources.size() > 0)
    {
        int count = 0;
        for(auto* i : sources)
        {
            auto n = getWidth() / 9.0f;
            auto centerBounds = juce::Rectangle<float> {1.5f * n, 1.5f * n, 6.0f * n, 6.0f * n};
            auto sliderBounds = centerBounds.expanded(0.35f * n).toNearestInt();
            sliders[count]->setBounds(sliderBounds);
            auto color = targetColors.atIndex(count);
            i->setBackground(color);
            i->setBounds(0, 0, getWidth(), getHeight());
            i->resized();
            i->buttonsToFront();
            if(i == selectedGroup)
                validSelected = true;
            ++count;
        }
    }
    mTarget.setBounds(getWidth() / 3, getHeight() / 3, getHeight() / 3, getHeight() / 3);
    mTarget.toFront(true);
    if(validSelected)
    {
        selectedGroup->toBehind(&mTarget);
        
        selectedSlider->toBehind(&mTarget);
    }
    
   
}


void ModTargetComponent::paint(juce::Graphics &g)
{

}

ModTargetSlider::ModTargetSlider(juce::DragAndDropContainer* c,
                std::string d,
                double min,
                double max,
                double def) :
numSources(0),
mTarget(c, this, d, min, max, def),
container(c)
{
    setInterceptsMouseClicks(true, true);
    selectedGroup = nullptr;
    selectedSlider = nullptr;
    addAndMakeVisible(&mTarget);
    mTarget.toFront(true);
    targetColors.add(Color::monochromeFrom(Color::RGBColor(169, 179, 193)));
}

void ModTargetSlider::buttonClicked(juce::Button *b)
{
    SelectorButton* sel;
    RemoveButton* rem;
    if((sel = dynamic_cast<SelectorButton*>(b)))
    {
        auto src = dynamic_cast<SourceButtonGroup*>(sel->getParentComponent());
        selectedGroup = src;
        selectedSlider = sliders[sources.indexOf(src)];
        printf("Group %d selected\n", sources.indexOf(selectedGroup));
    }
    else if ((rem = dynamic_cast<RemoveButton*>(b)))
    {
        auto src = dynamic_cast<SourceButtonGroup*>(rem->getParentComponent());
            if(selectedGroup == src)
            {
                if(sources.size() > 0)
                {
                    selectedGroup = sources.getLast();
                    selectedSlider = sliders[sources.indexOf(selectedGroup)];
                }
                else
                {
                    selectedGroup = nullptr;
                    selectedSlider = nullptr;
                }
            }
            auto index = sources.indexOf(src);
            sliders.remove(index);
            sources.removeObject(src);
            --numSources;
            int ind = 1;
            for(auto i : sources)
            {
                i->setIndex(ind);
                ++ind;
            }
            repaint();
    }
    else
    {
        if(sources.size() == 0) //don't need to check for duplicates if there are 0 sources
        {
            ++numSources;
            sources.add(new SourceButtonGroup(mTarget.getNewSource(), numSources, this));
            sliders.add(new DepthSlider(mTarget.getNewSource()));
            addAndMakeVisible(sources.getLast());
            addAndMakeVisible(sliders.getLast());
        }
        else //check for duplicates
        {
            bool added = false;
            for(auto i : sources)
                if(i->getId() == mTarget.getNewSource()->getId())
                    added = true;
            if(!added)
            {
                ++numSources;
                sources.add(new SourceButtonGroup(mTarget.getNewSource(), numSources, this));
                sliders.add(new DepthSlider(mTarget.getNewSource()));
                addAndMakeVisible(sources.getLast());
                addAndMakeVisible(sliders.getLast());
            }
        }
    }
    resized();
}

void ModTargetSlider::resized()
{
    bool validSelected = false;
    auto n = getWidth() / 9.0f;
    auto centerBounds = juce::Rectangle<float> {1.75f * n, 1.75f * n, 5.5f * n, 5.5f * n};
    if(sources.size() > 0)
    {
        int count = 0;
        for(auto* i : sources)
        {
            auto sliderBounds = centerBounds.toNearestInt().expanded(0.4f * n);
            sliders[count]->setBounds(sliderBounds);
            
            i->setBounds(0, 0, getWidth(), getHeight());
            i->resized();
            i->buttonsToFront();
            if(i == selectedGroup)
                validSelected = true;
            ++count;
        }
    }
    mTarget.setBounds(centerBounds.reduced(0.5f * n).toNearestInt());
    mTarget.toFront(true);
    if(validSelected)
    {
        selectedGroup->toBehind(&mTarget);
        
        selectedSlider->toBehind(&mTarget);
    }
}

