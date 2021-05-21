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
#include "ModulationSystem.h"

#define TABLESIZE 2048
#define FFT_ORDER 11
#define TABLES_PER_FRAME 10

#define SEMITONE_RATIO 1.05946309436f
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
    juce::File getSource() {return srcFile; }
private:
    juce::OwnedArray<WavetableFrame> frames;
    double sampleRate;
    float phase;
    float phaseDelta;
    int topIndex, bottomIndex;
    WavetableFrame* topFrame;
    WavetableFrame* bottomFrame;
    float topSample, bottomSample, skew;
    const juce::File srcFile;
    //===unison stuff================
    double lastHz;
    double stepUpHz;
    double stepDownHz;
    float mainSample;
    bool unisonMode;
    int unisonVoices;
    float unisonSpread;
    float unisonLevel;
    int uIdx;
    std::array<float, UNISON_MAX> uPhasesUpper;
    std::array<float, UNISON_MAX> uDeltasUpper;
    std::array<float, UNISON_MAX> uOutputsUpper;
    std::array<float, UNISON_MAX> uPhasesLower;
    std::array<float, UNISON_MAX> uDeltasLower;
    std::array<float, UNISON_MAX> uOutputsLower;
public:
    
    void setUnisonVoices(int voices)
    {
        unisonVoices = voices;
        setUnisonDeltas();
    }
    void setUnisonLevel(float level)
    {
        unisonLevel = level;
    }
    void setUnisonSpread(float value)
    {
        unisonSpread = value;
        setUnisonDeltas();
    }
    void setUnisonDeltas()
    {
        auto uSpread = ((stepUpHz * unisonSpread) - lastHz) / unisonVoices;
        int idx = 0;
        for(idx = 0; idx < unisonVoices; ++idx)
        {
            uDeltasUpper[idx] = (lastHz + (uSpread * idx)) / sampleRate;
        }
        auto lSpread = (lastHz - (stepDownHz * unisonSpread)) / unisonVoices;
        for(idx = 0; idx < unisonVoices; ++idx)
        {
            uDeltasLower[idx] = (lastHz - (lSpread * idx)) / sampleRate;
            ++idx;
        }
    }
    float unisonUpper(int tIndex, int bIndex, int idx, float position)
    {
        skew = (position * (numFrames - 1)) - bIndex;
        uPhasesUpper[idx] += uDeltasUpper[idx];
        if(uPhasesUpper[idx] > 1.0f)
            uPhasesUpper[idx] -= 1.0f;
        topSample = frames[tIndex]->getSample(uPhasesUpper[idx], uDeltasUpper[idx] * sampleRate);
        bottomSample = frames[bIndex]->getSample(uPhasesUpper[idx], uDeltasUpper[idx] * sampleRate);
        uOutputsUpper[idx] = ((bottomSample + ((topSample - bottomSample) * skew)) * unisonLevel) / (float)unisonVoices;
        return uOutputsUpper[idx];
    }
    float unisonLower(int tIndex, int bIndex, int idx, float position)
    {
        skew = (position * (numFrames - 1)) - bIndex;
        uPhasesLower[idx] += uDeltasLower[idx];
        if(uPhasesLower[idx] > 1.0f)
            uPhasesLower[idx] -= 1.0f;
        topSample = frames[tIndex]->getSample(uPhasesLower[idx], uDeltasLower[idx] * sampleRate);
        bottomSample = frames[bIndex]->getSample(uPhasesLower[idx], uDeltasLower[idx] * sampleRate);
        uOutputsLower[idx] = ((bottomSample + ((topSample - bottomSample) * skew)) * unisonLevel) / (float)unisonVoices;
        return uOutputsLower[idx];
    }
    void setUnisonMode(bool shouldHaveUnison)
    {
        unisonMode = shouldHaveUnison;
        if(unisonMode && unisonVoices == 0)
        {
            setUnisonVoices(1);
        }
    }
    float unisonSample(int tIndex, int bIndex, float input, float position)
    {
        input *= (1.0f - unisonLevel);
        for(uIdx = 0; uIdx < unisonVoices; ++uIdx)
        {
            input += unisonLower(tIndex, bIndex, uIdx, position);
            input += unisonUpper(tIndex, bIndex, uIdx, position);
        }
        return input;
    }
    
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
    float getSample(double hz)
    {
        modEnv.clockOutput();
        return ampEnv.process(pOsc->getSample(hz, position) * level);
    }
    doubleVec getGraphData(int resolution) {return pOsc->getGraphData(resolution); }
    int getNumFrames() {return pOsc->numFrames; }
    void setPosition(float newPos) {position = newPos;}
    void setLevel(float newLevel) {level = newLevel;}
    float lastAmpEnv() {return ampEnv.output; }
    float lastModEnv() {return modEnv.output; }
    DAHDSR ampEnv;
    DAHDSR modEnv;
    juce::File getSource() {return pOsc->getSource(); }
private:
    float position;
    float level;
    std::unique_ptr<WavetableOscCore> pOsc;
    int unisonVoices;
    float unisonSpread;
    float unisonLevel;
    bool unisonMode;
public:
    void setUnisonVoices(int numVoices)
    {
        unisonVoices = numVoices;
        pOsc->setUnisonVoices(unisonVoices);
    }
    void setUnisonMode(bool mode)
    {
        unisonMode = mode;
        pOsc->setUnisonMode(unisonMode);
    }
    void setUnisonSpread(float spread)
    {
        unisonSpread = spread;
        pOsc->setUnisonSpread(unisonSpread);
    }
    void setUnisonLevel(float level)
    {
        unisonLevel = level;
        pOsc->setUnisonLevel(unisonLevel);
    }
};
