/*
  ==============================================================================

    DAHDSRGraph.cpp
    Created: 8 Oct 2020 10:33:41am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "DAHDSRGraph.h"

DAHDSRGraph::DAHDSRGraph(juce::Slider* Delay,
                         juce::Slider* Attack,
                         juce::Slider* Hold,
                         juce::Slider* Decay,
                         juce::Slider* Sustain,
                         juce::Slider* Release):
sDelay(Delay),
sAttack(Attack),
sHold(Hold),
sDecay(Decay),
sSustain(Sustain),
sRelease(Release)
{
    sDelay->addListener(this);
    sAttack->addListener(this);
    sHold->addListener(this);
    sDecay->addListener(this);
    sSustain->addListener(this);
    sRelease->addListener(this);
    fDelay = 0.0f;
    fAttack = 20.0f;
    fHold = 0.0f;
    fDecay = 100.0f;
    fSustain = 0.6f;
    fRelease = 40.0f;
    bgColor = juce::Colours::black.brighter(0.1f);
    traceColor = Color::RGBColor(255, 236, 95);
}

void DAHDSRGraph::grabValues()
{
    auto exp = 1.0f / 2.0f;
    if(sAttack->getValue() >= 0.0f)
    {
        fDelay = pow(sDelay->getValue(), exp);
        fAttack = pow(sAttack->getValue(), exp);
        fHold = pow(sHold->getValue(), exp);
        fDecay = pow(sDecay->getValue(), exp);
        fSustain = sSustain->getValue();
        fRelease = pow(sRelease->getValue(), exp);
    }
}

void DAHDSRGraph::paint(juce::Graphics &g)
{
    
    g.setColour(bgColor);
    
    auto area = getLocalBounds().toFloat().reduced(3.0f);
    g.fillRect(area);
    auto timeTotal = fDelay + fAttack + fHold + fDecay + fRelease;
    juce::Path trace;
    trace.startNewSubPath(0.0f, area.getHeight());
    trace.lineTo(fDelay, area.getHeight());
    trace.lineTo(fDelay + fAttack, 0.0f);
    trace.lineTo(fDelay + fAttack + fHold, 0.0f);
    auto sustainY = (1.0f - fSustain) * area.getHeight();
    
    auto sustainLength = timeTotal * 0.25;
    trace.lineTo(fDelay + fAttack + fHold + fDecay, sustainY);
    trace.lineTo(fDelay + fAttack + fHold + fDecay + sustainLength, sustainY);
    trace.lineTo(timeTotal + sustainLength, area.getHeight());
    trace.scaleToFit(area.getX(), 5.0f, area.getWidth(), (area.getHeight() - 1.0f), false);
    
    auto stroke1 = juce::PathStrokeType(1.0f);
    g.setColour(traceColor);
    g.strokePath(trace, stroke1);
    auto iBounds = area.reduced(1.0f);
    g.setColour(bgColor.brighter(0.2f));
    auto stroke2 = juce::PathStrokeType(3.0f);
    juce::Path bottom;
    bottom.startNewSubPath(iBounds.getX(), iBounds.getBottom());
    bottom.lineTo(iBounds.getRight(), iBounds.getBottom());
    bottom.closeSubPath();
    g.strokePath(bottom, stroke2);
}
