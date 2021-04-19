/*
  ==============================================================================

    WavetableProcessor.h
    Created: 1 Mar 2021 12:50:20pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#define TABLESIZE 2048
#define NUMTABLES 32

#define WTPOS_MIN 0.0f
#define WTPOS_MAX 1.0f
#define WTPOS_DEFAULT 0.0f



struct WaveTable
{
    WaveTable(int size, double fMin, double fMax, double* input) : minFreq(fMin), maxFreq(fMax), length(size), table(new float [size])
    {
        double accum = 0.0f;
        for(int i = 0; i < length; ++i)
        {
            table[i] = input[i];
            if(i < length / 2)
                accum += table[i];
        }
        //the average sample value for the first half of the wave
        if(accum / (double)(length / 2.0f) < 0.0f) //if the wave starts on a negative phase, invert it so they always match
        {
            for(int i = 0; i < size; ++i)
                table[i] *= -1.0f;
        }
    }
    //construct empty table of zeroes
    WaveTable(int size=TABLESIZE, double fMin=0.0f, double fMax=0.0f) : minFreq(fMin), maxFreq(fMax), length(size), table(new float [size])
    {
        for(int i = 0; i < length; ++i)
        {
            table[i] = 0.0f;
        }
    }
    ~WaveTable()
    {
        delete [] table;
    }
    float& operator [] (int idx)
    {
        return table[idx];
    }
    void normalize(float amp=1.0f)
    {
        auto maxLevel = std::numeric_limits<float>::min();
        auto minLevel = std::numeric_limits<float>::max();
        for(int i = 0; i < length; ++i)
        {
            if(table[i] < minLevel)
                minLevel = table[i];
            if(table[i] > maxLevel)
                maxLevel = table[i];
        }
        for(int i = 0; i < length; ++i)
            table[i] /= (fabs(maxLevel) + fabs(minLevel)) / 2.0f;
    }
    void makeDerivative()
    {
        dTable = new float[length];
        auto dX = juce::MathConstants<double>::twoPi / length;
        float aSample, bSample, rise;
        for(int i = 0; i < length; ++i)
        {
            aSample = table[i];
            bSample = (i < length - 1) ? table[i + 1] : table[0];
            rise = bSample - aSample;
            dTable[i] = rise / dX;
        }
    }
    double minFreq;
    double maxFreq; //max frequency this table can use before aliasing
    int length; //number of samples
    float* table;
    float* dTable;
};

class WavetableFrame
{
public:
    WavetableFrame(std::vector<float> t=std::vector<float>(TABLESIZE, 0.0f));
    WavetableFrame(std::array<float, TABLESIZE> t);
    WavetableFrame(float* t, int length);
    ~WavetableFrame(){}
    void setSampleRate(double rate)
    {
        sampleRate = rate;
    }
    int createTables(double* waveReal, double* waveImag, int numSamples);
    float makeTable(double* waveReal, double* waveImag, int numSamples, double scale, double bottomFreq, double topFreq);
    float getSample() {return output;}
    float getSample(float pos, double frequency)
    {
        phaseDelta = (double)(frequency / sampleRate);
        auto table = tableForFreq(phaseDelta);
        phase = pos;
        phase += phaseDelta;
        if(phase > 1.0f)
        {
            phase -= 1.0f;
        }
        bottomSampleIndex = (int)(table->length * phase);
        skew = (table->length * phase) - bottomSampleIndex;
        sampleDiff = table->table[bottomSampleIndex + 1] - table->table[bottomSampleIndex];
        output = table->table[bottomSampleIndex] + (skew * sampleDiff);
        return output;
    }
    float getSample(float phase_)
    {
        phase = phase_;
        phaseDelta = fabs(phase - lastPhase);
        auto table = tableForFreq(phaseDelta);
        bottomSampleIndex = (int)(table->length * phase);
        skew = (table->length * phase) - bottomSampleIndex;
        sampleDiff = table->table[bottomSampleIndex + 1] - table->table[bottomSampleIndex];
        output = table->table[bottomSampleIndex] + (skew * sampleDiff);
        lastPhase = phase;
        return output;
    }
    WaveTable* tableForFreq(double frequency);
    std::vector<float> getBasicVector(int resolution);
    void clockSample(double frequency);
private:
    WaveTable* pTables;
    WaveTable* rawTable;
    double lastMinFreq;
    int tablesAdded;
    std::vector<float> data; //the initial input data from which all the tables are made
    float phase;
    float phaseDelta;
    float lastPhase;
    float output;
    double sampleRate;
    int bottomSampleIndex;
    float sampleDiff;
    float skew;
};

class WavetableOsc
{
public:
    WavetableOsc(std::vector<float> firstFrameData)
    {
        phase = 0.0f;
        sampleRate = 44100.0f;
        currentPosition = 0.0f;
        numFrames = 0;
    }
    WavetableOsc(juce::File wavData);
    ~WavetableOsc() {}
    void replaceTables(juce::String nTables);
    void setSampleRate(double newRate)
    {
        sampleRate = newRate;
        for(int i = 0; i < numFrames; ++i)
        {
            frames[i].setSampleRate(sampleRate);
        }
    }
    void setPosition(float p)
    {
        targetPosition = p;
        maxSamplePosDelta = 3.0f / sampleRate;
        if(currentPosition > targetPosition)
            posDeltaSign = -1.0f;
        else
            posDeltaSign = 1.0f;
    }
    static float clamp(float input, float amplitude)
    {
        if(input < (-1.0f * amplitude))
            return -1.0f * amplitude;
        if(input > amplitude)
            return amplitude;
        return input;
    }
    float getSample(double freq)
    {
        phaseDelta = (float)freq / sampleRate;
        phase += phaseDelta;
        if(phase > 1.0f)
            phase -= 1.0f;
        if(fabs(targetPosition - currentPosition) > maxSamplePosDelta)
                currentPosition = currentPosition + (maxSamplePosDelta * posDeltaSign);
        else
            currentPosition = targetPosition;
        pFrame = currentPosition * (numFrames - 1);
        lowerIndex = floor(pFrame);
        skew = pFrame - lowerIndex;
        upperIndex = (lowerIndex == (numFrames - 1)) ? 0 : lowerIndex + 1;
        bSample = frames[lowerIndex].getSample(phase);
        tSample = frames[upperIndex].getSample(phase);
        output = bSample + ((tSample - bSample) * skew);
        return output;
    }
    std::vector<std::vector<float>> getDataToGraph(int resolution);
    juce::StringArray waveNames;
    float currentPosition;
private:
    float phase;
    float phaseDelta;
    int frameIndex;
    int lowerIndex;
    int upperIndex;
    float targetPosition;
    float maxSamplePosDelta; //maximum change to the position value per sample period
    float posDeltaSign;
    float pFrame;
    int numFrames;
    float skew;
    float tSample;
    float bSample;
    float output;
    double sampleRate;
    WavetableFrame* frames;
};

class WavetableOscHolder
{
public:
    WavetableOscHolder(juce::File file) :
    osc(std::make_unique<WavetableOsc>(file))
    {
        printf("Oscillator created\n");
    }
    void replaceFromFile(juce::File file)
    {
        osc.reset(new WavetableOsc(file));
    }
    void setPosition(float pos) {osc->setPosition(pos);}
    void setSampleRate(double rate) {osc->setSampleRate(rate);}
    float getSample(double freq)
    {
        return osc->getSample(freq);
    }
    std::vector<std::vector<float>> getDataToGraph(int resolution) {return osc->getDataToGraph(resolution);}
    juce::StringArray* waveNames;
    juce::Array<juce::File> waveFiles;
    float getPosition() {return osc->currentPosition;}
private:
    std::unique_ptr<WavetableOsc> osc;
};

