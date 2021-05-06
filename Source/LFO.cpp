/*
  ==============================================================================

    LFO.cpp
    Created: 4 May 2021 12:02:42pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "LFO.h"

OctaneLFO::OctaneLFO() :
sampleRate(44100.0f),
phase(0.0f),
phaseDelta(0.0f),
triggerMode(false),
lastOutput(0.0f)
{
    auto dPhase = juce::MathConstants<float>::twoPi / (float)LFO_POINTS;
    for(int i = 0; i < LFO_POINTS; ++i)
    {
        points[i] = (std::sin(i * dPhase) / 2.0f) + 0.5f;
        //printf("Value %d: %f\n", i, points[i]);
    }
    setRateHz(1.0f);
}

void OctaneLFO::setRateHz(double hz)
{
    phaseDelta = (float)hz / sampleRate;
}

void OctaneLFO::noteOn()
{
    if(triggerMode)
    {
        phaseReset();
    }
}
void OctaneLFO::noteOff()
{
    
}

void OctaneLFO::setTrigger(bool shouldTrigger)
{
    triggerMode = shouldTrigger;
}

void OctaneLFO::tick()
{
    phase += phaseDelta;
    if(phase > 1.0f)
        phase -= 1.0f;
}

float OctaneLFO::getOutput()
{
    tick();
    lowerIndex = floor(LFO_POINTS * phase);
    lowerVal = points[lowerIndex];
    skew = (LFO_POINTS * phase) - lowerIndex;
    upperVal = (lowerIndex == LFO_POINTS - 1) ? points[0] : points[lowerIndex + 1];
    lastOutput = lowerVal + (skew * (upperVal - lowerVal));
    return lastOutput;
}
