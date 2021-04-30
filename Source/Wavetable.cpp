/*
  ==============================================================================

    Wavetable.cpp
    Created: 30 Apr 2021 2:09:05pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Wavetable.h"

WavetableFrame::WavetableFrame(std::array<float, TABLESIZE>& firstTable)
{
    for(int i = 0; i < TABLES_PER_FRAME; ++i)
    {
        tables.add(new Wavetable()); //fill the OwnedArray with empty tables
    }
}
