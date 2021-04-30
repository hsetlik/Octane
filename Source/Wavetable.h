/*
  ==============================================================================

    Wavetable.h
    Created: 30 Apr 2021 2:09:05pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#define TABLESIZE 2048
#define FFT_ORDER 11
#define TABLES_PER_FRAME 10
struct Wavetable
{
    float* table;
    float minFreq;
    float maxFreq;
};

class WavetableFrame
{
public:
    WavetableFrame(std::array<float, TABLESIZE>& firstTable);
    float getSample(float phase, double hz);
    std::vector<float> getGraphData(int resolution);
    void setSampleRate(double rate)
    {
        sampleRate = rate;
    }
private:
    juce::OwnedArray<Wavetable> tables;
    double sampleRate;
};
