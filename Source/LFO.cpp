/*
  ==============================================================================

    LFO.cpp
    Created: 4 May 2021 12:02:42pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "LFO.h"

OctaneLFO::OctaneLFO(lfoFunc srcFunction) :
sampleRate(44100.0f),
phase(0.0f),
phaseDelta(0.0f),
isPlaying(true),
triggerMode(false)
{
    srcFunction(points);
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
        isPlaying = true;
    }
}
void OctaneLFO::noteOff()
{
    if(triggerMode)
    {
        isPlaying = false;
    }
}

void OctaneLFO::setTrigger(bool shouldTrigger)
{
    triggerMode = shouldTrigger;
    if(triggerMode)
    {
        isPlaying = false;
        phaseReset();
    }
}

void OctaneLFO::tick()
{
    if(isPlaying)
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
    return(lowerVal + (skew * (upperVal - lowerVal)));
}
