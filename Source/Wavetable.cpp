/*
  ==============================================================================

    Wavetable.cpp
    Created: 30 Apr 2021 2:09:05pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Wavetable.h"

WavetableFrame::WavetableFrame(std::array<float, TABLESIZE>& firstTable) : sampleRate(44100.0f), tablesAdded(0)
{
    for(int i = 0; i < TABLES_PER_FRAME; ++i)
    {
        tables.add(new Wavetable()); //fill the OwnedArray with empty tables
    }
    float* fReal = new float[TABLESIZE];
    float* fImag = new float[TABLESIZE];
    for(int i = 0; i < TABLESIZE; ++i)
    {
        fReal[i] = 0.0f;
        fImag[i] = firstTable[i];
    }
    FFT::runFloat(TABLESIZE, fReal, fImag);
    createTables(TABLESIZE, fReal, fImag);
}

WavetableFrame::WavetableFrame(const WavetableFrame& orig) :
sampleRate(orig.sampleRate)
{
    for(int i = 0; i < TABLES_PER_FRAME; ++i)
    {
        tables.add(new Wavetable());
        *tables[i] = *orig.tables[i];
    }
}

void WavetableFrame::createTables(int size, float *real, float *imag)
{
    int idx;
    // zero DC offset and Nyquist (set first and last samples of each array to zero, in other words)
    real[0] = imag[0] = 0.0f;
    real[size >> 1] = imag[size >> 1] = 0.0f;
    int maxHarmonic = size >> 1;
    const double minVal = 0.000001f;
    while((fabs(real[maxHarmonic]) + fabs(imag[maxHarmonic]) < minVal) && maxHarmonic)
        --maxHarmonic;
    float topFreq = (float)(2.0f / 3.0f / maxHarmonic); //note:: topFreq is in units of phase fraction per sample, not Hz
    float* ar = new float[size];
    float* ai = new float[size];
    float scale = 0.0f;
    float lastMinFreq = 0.0f;
    while(maxHarmonic)
    {
        // fill the table in with the needed harmonics
        for (idx = 0; idx < size; idx++)
            ar[idx] = ai[idx] = 0.0f;
        for (idx = 1; idx <= maxHarmonic; idx++)
        {
            ar[idx] = real[idx];
            ai[idx] = imag[idx];
            ar[size - idx] = real[size - idx];
            ai[size - idx] = imag[size - idx];
        }
        // make the wavetable
        scale = makeTable(ar, ai, size, scale, lastMinFreq, topFreq);
        lastMinFreq = topFreq;
        topFreq *= 2.0f;
        maxHarmonic >>= 1;
    }
}

float WavetableFrame::makeTable(float *waveReal, float *waveImag, int numSamples, float scale, float bottomFreq, float topFreq)
{
    tables[tablesAdded]->maxFreq = topFreq;
    tables[tablesAdded]->minFreq = bottomFreq;
    FFT::runFloat(numSamples, waveReal, waveImag);
    if (scale == 0.0f)
    {
        // get maximum value to scale to -1 - 1
        double max = 0.0f;
        for (int idx = 0; idx < numSamples; idx++)
        {
            double temp = fabs(waveImag[idx]);
            if (max < temp)
                max = temp;
        }
        scale = 1.0f / max * 0.999f;
        //printf("Table: %d has scale: %f\n", tablesAdded, scale);
    }
    auto minLevel = std::numeric_limits<float>::max();
    auto maxLevel = std::numeric_limits<float>::min();
    for(int i = 0; i < numSamples; ++i)
    {
        tables[tablesAdded]->table[i] = waveImag[i] * scale;
        if(tables[tablesAdded]->table[i] < minLevel)
            minLevel = tables[tablesAdded]->table[i];
        if(tables[tablesAdded]->table[i] > maxLevel)
            maxLevel = tables[tablesAdded]->table[i];
    }
    auto offset = maxLevel + minLevel;
    minLevel = std::numeric_limits<float>::max();
    maxLevel = std::numeric_limits<float>::min();
    for(int i = 0; i < numSamples; ++i)
    {
        tables[tablesAdded]->table[i] -= (offset / 2.0f); //make sure each table has no DC offset
        if(tables[tablesAdded]->table[i] < minLevel)
            minLevel = tables[tablesAdded]->table[i];
        if(tables[tablesAdded]->table[i] > maxLevel)
            maxLevel = tables[tablesAdded]->table[i];
    }
    ++tablesAdded;
    return (float)scale;
}

std::vector<float> WavetableFrame::getGraphData(int resolution)
{
    auto inc = TABLESIZE / resolution;
    std::vector<float> data;
    for(int i = 0; i < resolution; ++i)
    {
        auto idx = (int) i * inc;
        float value = tables[0]->table[idx]; //use the first table with the most harmonic detail for graphing
        data.push_back(value);
    }
    return data;
}

Wavetable* WavetableFrame::tableForHz(double hz)
{
    phaseDelta = (float) hz / sampleRate;
    for(auto table : tables)
    {
        if(table->maxFreq > phaseDelta && table->minFreq <= phaseDelta)
            return table;
    }
    return tables.getLast();
}

float WavetableFrame::getSample(float phase, double hz)
{
    auto table = tableForHz(hz);
    bottomIndex = floor(phase * TABLESIZE);
    skew = (phase * TABLESIZE) - bottomIndex;
    bSample = table->table[bottomIndex];
    tSample = (bottomIndex == TABLESIZE - 1) ? table->table[0] : table->table[bottomIndex + 1];
    return bSample + ((tSample - bSample) * skew);
}

//=================================================================================================

WavetableOscCore::WavetableOscCore(juce::File src) :
numFrames(0),
sampleRate(44100.0f),
phase(0.0f),
phaseDelta(0.0f),
srcFile(src),
lastHz(0.0f),
stepUpHz(0.0f),
stepDownHz(0.0f),
unisonMode(false),
unisonVoices(0),
unisonLevel(0.0f)
{
    uPhasesLower.fill(0.0f);
    uDeltasLower.fill(0.0f);
    uOutputsLower.fill(0.0f);
    uPhasesUpper.fill(0.0f);
    uDeltasUpper.fill(0.0f);
    uOutputsUpper.fill(0.0f);
    auto manager = new juce::AudioFormatManager();
    manager->registerBasicFormats();
    auto reader = manager->createReaderFor(src);
    auto numSamples = reader->lengthInSamples;
    numFrames = (int)(numSamples / TABLESIZE);
    std::array<float, TABLESIZE> fArray; //container to move data from audio buffers to wavetables
    long currentSample = 0;
    auto buffer = juce::AudioBuffer<float>(1, TABLESIZE);
    reader->read(&buffer, 0, TABLESIZE, currentSample, true, true); //fill the buffer for the first frame
    for(int i = 0; i < numFrames; ++i)
    {
        for(int sample = 0; sample < TABLESIZE; ++sample)
        {
            fArray[sample] = buffer.getSample(0, sample); //transfer each sample into fArray
        }
        frames.add(new WavetableFrame(fArray));
        buffer.clear();
        currentSample += TABLESIZE;
        reader->read(&buffer, 0, TABLESIZE, currentSample, true, true);
    }
    delete reader;
    delete manager;
}

WavetableOscCore::WavetableOscCore(const WavetableOscCore& orig) :
numFrames(orig.numFrames),
sampleRate(orig.sampleRate),
phase(0.0f),
phaseDelta(0.0f),
srcFile(orig.srcFile),
lastHz(0.0f),
stepUpHz(0.0f),
stepDownHz(0.0f),
unisonMode(false),
unisonVoices(0),
unisonLevel(0.0f)
{
    uPhasesLower.fill(0.0f);
    uDeltasLower.fill(0.0f);
    uOutputsLower.fill(0.0f);
    uPhasesUpper.fill(0.0f);
    uDeltasUpper.fill(0.0f);
    uOutputsUpper.fill(0.0f);
    for(int i = 0; i < orig.numFrames; ++i)
    {
        frames.add(new WavetableFrame(*orig.frames[i]));
    }
}

void WavetableOscCore::setSampleRate(double newRate)
{
    sampleRate = newRate;
    for(auto frame : frames)
    {
        frame->setSampleRate(newRate);
    }
}

doubleVec WavetableOscCore::getGraphData(int resolution)
{
    doubleVec data;
    for(auto frame : frames)
    {
        data.push_back(frame->getGraphData(resolution));
    }
    return data;
}
float WavetableOscCore::getSample(double hz, float position)
{
    if(lastHz != hz)
    {
        lastHz = hz;
        stepUpHz = lastHz * pow(SEMITONE_RATIO, 2);
        stepDownHz = lastHz / pow(SEMITONE_RATIO, 2);
    }
    if(position > 1.0f)
        position = 1.0f;
    bottomIndex = floor(position * (numFrames - 1));
    topIndex = (bottomIndex >= numFrames - 1) ? 0 : bottomIndex + 1;
    skew = (position * (numFrames - 1)) - bottomIndex;
    phaseDelta = hz / sampleRate;
    phase += phaseDelta;
    if(phase > 1.0f)
        phase -= 1.0f;
    topSample = frames[topIndex]->getSample(phase, hz);
    bottomSample = frames[bottomIndex]->getSample(phase, hz);
    mainSample = bottomSample + ((topSample - bottomSample) * skew);
    if(!unisonMode)
        return mainSample;
    return unisonSample(topIndex, bottomIndex, mainSample, position);
}

//============================================================================================

OctaneOsc::OctaneOsc(juce::File src) :
position(0.0f),
level(1.0f),
pOsc(std::make_unique<WavetableOscCore>(src)),
unisonVoices(0),
unisonSpread(0.0f),
unisonLevel(0.0f),
unisonMode(false)
{
   
}

OctaneOsc:: OctaneOsc(const OctaneOsc& orig) :
position(orig.position),
level(orig.level),
pOsc(std::make_unique<WavetableOscCore>(*orig.pOsc)),
unisonVoices(orig.unisonVoices),
unisonSpread(orig.unisonSpread),
unisonLevel(orig.unisonLevel),
unisonMode(orig.unisonMode)
{
    
}

void OctaneOsc::replace(juce::File src)
{
    pOsc.reset(new WavetableOscCore(src));
    if(unisonMode) //! so that unison settings are maintained when the osc core gets replaced
    {
        setUnisonMode(unisonMode);
        setUnisonVoices(unisonVoices);
        setUnisonSpread(unisonSpread);
        setUnisonLevel(unisonLevel);
    }
}

void OctaneOsc::setSampleRate(double rate)
{
    pOsc->setSampleRate(rate);
    ampEnv.setSampleRate(rate);
    modEnv.setSampleRate(rate);
}
