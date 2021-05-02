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
