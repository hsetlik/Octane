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
#include "DAHDSR.h"

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

using doubleVec = std::vector<std::vector<float>>;

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

class WavetableOscCore
{
public:
    WavetableOscCore(juce::File src);
    float getSample(double hz, float position);
    doubleVec getGraphData(int resolution);
    void setSampleRate(double newRate);
    int numFrames;
private:
    juce::OwnedArray<WavetableFrame> frames;
    double sampleRate;
    float position;
    float phase;
    float phaseDelta;
    int topIndex, bottomIndex;
    WavetableFrame* topFrame;
    WavetableFrame* bottomFrame;
    float topSample, bottomSample, skew;
};

class OctaneOsc
{
public:
    OctaneOsc(juce::File src);
    void triggerOn()
    {
        ampEnv.triggerOn();
        modEnv.triggerOn();
    }
    void triggerOff()
    {
        ampEnv.triggerOff();
        modEnv.triggerOff();
    }
    void replace(juce::File src);
    void setSampleRate(double rate);
    float getSample(double hz, float position) {return pOsc->getSample(hz, position); }
    float getSample(double hz)
    {
        modEnv.clockOutput();
        return ampEnv.process(pOsc->getSample(hz, position));
    }
    doubleVec getGraphData(int resolution) {return pOsc->getGraphData(resolution); }
    int getNumFrames() {return pOsc->numFrames; }
    float position;
    DAHDSR ampEnv;
    DAHDSR modEnv;
private:
    std::unique_ptr<WavetableOscCore> pOsc;
};
