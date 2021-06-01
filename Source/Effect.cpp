/*
  ==============================================================================

    Overdrive.cpp
    Created: 31 May 2021 5:10:47pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Effect.h"
std::vector<juce::String> Effect::effectStrings{"TrueClip", "BitCrusher"};


float OctaneEffectProcessor::process(float input)
{
    lastSample = input;
    for(auto effect : allEffects)
    {
        lastSample = effect->processWetDry(lastSample);
    }
    return lastSample;
}
