/*
  ==============================================================================

    StlUtil.h
    Created: 2 May 2021 12:36:11pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace StlUtil
{
template <typename T> void addIfUnique(std::vector<T>& vec, T obj)
{
    bool exists = false;
    for(auto test : vec)
    {
        if(test == obj)
        {
            exists = true;
            break;
        }
    }
    if(!exists)
    {
        vec.push_back(obj);
    }
}
template <typename T> bool existsIn(std::vector<T>& vec, T obj)
{
    for(auto test : vec)
    {
        if(test == obj)
            return true;
    }
    return false;
}

}

namespace MathUtil
{
    template <typename T> T onUpCurve(T input, T steepness)
{ //! the curve is upward as in the value will always be less than input
    auto e = juce::MathConstants<float>::euler;
    auto exp = std::powf(e, steepness * (input - 1.0f));
    auto offset = std::powf(e, steepness * -1.0f);
    return exp - offset;
}
};
