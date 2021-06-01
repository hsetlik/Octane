/*
  ==============================================================================

    Overdrive.cpp
    Created: 31 May 2021 5:10:47pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Effect.h"

float OctaneEffectProcessor::process(float input)
{
    lastSample = input;
    for(auto effect : allEffects)
    {
        lastSample = effect->process(lastSample);
    }
    return lastSample;
}
