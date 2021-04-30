/*
  ==============================================================================

    Wavetable.h
    Created: 30 Apr 2021 2:09:05pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "FFT.h"
#define TABLESIZE 2048
#define FFT_ORDER 11
#define TABLES_PER_FRAME 10
struct Wavetable
{
    Wavetable(int size)
    {
        table = new float [size];
    }
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
    void createTables(int size, float* real, float* imag);
    float makeTable(float* waveReal, float* waveImag, int numSamples, float scale, float bottomFreq, float topFreq);
    Wavetable* tableForHz(double hz);
private:
    juce::OwnedArray<Wavetable> tables;
    float phaseDelta;
    double sampleRate;
    int tablesAdded;
    int bottomIndex;
    float bSample;
    float tSample;
    float skew;
};

class WavetableOsc
{
public:
    WavetableOsc(juce::File src);
    float getSample(double hz, float position);
    int numFrames;
private:
    juce::OwnedArray<WavetableFrame> frames;
    float position;
    float phase;
    float phaseDelta;
};
