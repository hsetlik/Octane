/*
  ==============================================================================

    ModulationSystem.cpp
    Created: 1 May 2021 11:02:04am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "ModulationSystem.h"

float SynthParam::getActual()
{
    actualOut = baseValue;
    for(auto src : modSources)
    {
        actualOut += actualOffset(src);
    }
    return actualOut;
}

float SynthParam::getAdjusted()
{
    return (max - min) * getActual();
}

ModSource* SynthParam::makeSource(float d)
{
    return new ModSource(this, d);
}

float SynthParam::actualOffset(ModSource *mod)
{
    return (max - min) * (mod->src->getAdjusted() * mod->depth);
}

//=======================================================================

SynthParamStructure::SynthParamStructure()
{
    //TODO: this constructor needs to create all the possible parameters
}
