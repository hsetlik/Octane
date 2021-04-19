/*
  ==============================================================================

    SynthParameter.cpp
    Created: 18 Apr 2021 5:10:54pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "SynthParameter.h"
float SynthParam::clockModulation()
{
    offset = 0.0f;
    for(auto src : modSources)
    {
        temp = src->getOutput();
        if(temp >= 0.0f)
        {
            diff = lerp(value.load(), max.load(), temp);
        }
        if(temp < 0.0f)
        {
            diff = lerp(value.load(), min.load(), -temp); //diff should be negative in this case
        }
        offset += diff;
    }
    return offset;
}
