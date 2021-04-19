/*
  ==============================================================================

    WavetableProcessor.cpp
    Created: 1 Mar 2021 12:50:20pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "WavetableProcessor.h"

void fft(int N, double *ar, double *ai)
/*
 in-place complex fft
 
 After Cooley, Lewis, and Welch; from Rabiner & Gold (1975)
 
 program adapted from FORTRAN
 by K. Steiglitz  (ken@princeton.edu)
 Computer Science Dept.
 Princeton University 08544          */
{
    int i, j, k, L;            /* indexes */
    int M, TEMP, LE, LE1, ip;  /* M = log N */
    int NV2, NM1;
    double t;               /* temp */
    double Ur, Ui, Wr, Wi, Tr, Ti;
    double Ur_old;
    // if ((N > 1) && !(N & (N - 1)))   // make sure we have a power of 2
    NV2 = N >> 1;
    NM1 = N - 1;
    TEMP = N; /* get M = log N */
    M = 0;
    while (TEMP >>= 1)
        ++M;
    /* shuffle */
    j = 1;
    for (i = 1; i <= NM1; i++)
    {
        if(i<j)
        {             /* swap a[i] and a[j] */
            t = ar[j-1];
            ar[j-1] = ar[i-1];
            ar[i-1] = t;
            t = ai[j-1];
            ai[j-1] = ai[i-1];
            ai[i-1] = t;
        }
        k = NV2;             /* bit-reversed counter */
        while(k < j)
        {
            j -= k;
            k /= 2;
        }
        j += k;
    }
    LE = 1.0f;
    for (L = 1; L <= M; L++) {            // stage L
        LE1 = LE;                         // (LE1 = LE/2)
        LE *= 2;                          // (LE = 2^L)
        Ur = 1.0f;
        Ui = 0.0f;
        Wr = cos(M_PI/(float)LE1);
        Wi = sin(M_PI/(float)LE1); // Cooley, Lewis, and Welch have "+" here
        for (j = 1; j <= LE1; j++)
        {
            for (i = j; i <= N; i += LE)
            { // butterfly
                ip = i+LE1;
                Tr = ar[ip-1] * Ur - ai[ip-1] * Ui;
                Ti = ar[ip-1] * Ui + ai[ip-1] * Ur;
                ar[ip-1] = ar[i-1] - Tr;
                ai[ip-1] = ai[i-1] - Ti;
                ar[i-1]  = ar[i-1] + Tr;
                ai[i-1]  = ai[i-1] + Ti;
            }
            Ur_old = Ur;
            Ur = Ur_old * Wr - Ui * Wi;
            Ui = Ur_old * Wi + Ui * Wr;
        }
    }
}

WavetableFrame::WavetableFrame(std::vector<float> t) : pTables(new WaveTable [10]), tablesAdded(0), data(t), phase(0.0f), sampleRate(44100.0f)
{
    lastMinFreq = 0.0f;
    auto inc = float(data.size() / TABLESIZE);
    float bottomSample, topSample, difference, skew, sample;
    double pos;
    double* freqWaveReal = new double[TABLESIZE];
    double* freqWaveImag = new double[TABLESIZE];
    for(int i = 0; i < TABLESIZE; ++i)
    {
        pos = inc * i;
        bottomSample = data[floor(pos)];
        topSample = data[ceil(pos)];
        difference = topSample - bottomSample;
        skew = pos - floor(pos);
        sample = bottomSample + (skew * difference);
        //setting up FFT arrays
        freqWaveImag[i] = (double)sample;
        freqWaveReal[i] = 0.0f;
    }
    //FFT time!!
    fft(TABLESIZE, freqWaveReal, freqWaveImag);
    createTables(freqWaveReal, freqWaveImag, TABLESIZE);
    delete [] freqWaveReal;
    delete [] freqWaveImag;
}

WavetableFrame::WavetableFrame(std::array<float, TABLESIZE> t) : pTables(new WaveTable [10]), tablesAdded(0), data(t.begin(), t.end()), phase(0.0f), sampleRate(44100.0f)
{
    lastMinFreq = 0.0f;
    auto inc = float(data.size() / TABLESIZE);
    float bottomSample, topSample, difference, skew, sample;
    double pos = 0.0f;
    double* freqWaveReal = new double[TABLESIZE];
    double* freqWaveImag = new double[TABLESIZE];
    for(int i = 0; i < TABLESIZE; ++i)
    {
        pos = inc * i;
        bottomSample = data[floor(pos)];
        topSample = data[ceil(pos)];
        difference = topSample - bottomSample;
        skew = pos - floor(pos);
        sample = bottomSample + (skew * difference);
        //setting up FFT arrays
        freqWaveImag[i] = (double)sample;
        freqWaveReal[i] = 0.0f;
    }
    fft(TABLESIZE, freqWaveReal, freqWaveImag);
    createTables(freqWaveReal, freqWaveImag, TABLESIZE);
    delete [] freqWaveReal;
    delete [] freqWaveImag;
}

WavetableFrame::WavetableFrame(float* t, int length) : pTables(new WaveTable [10]), tablesAdded(0), phase(0.0f), sampleRate(44100.0f)
{
    for(int i = 0; i < length; ++i)
    {
        data.push_back(t[i]);
    }
    lastMinFreq = 0.0f;
    auto inc = float(data.size() / TABLESIZE);
    float bottomSample, topSample, difference, skew, sample;
    double pos = 0.0f;
    double* freqWaveReal = new double[TABLESIZE];
    double* freqWaveImag = new double[TABLESIZE];
    for(int i = 0; i < TABLESIZE; ++i)
    {
        pos = inc * i;
        bottomSample = data[floor(pos)];
        topSample = data[ceil(pos)];
        difference = topSample - bottomSample;
        skew = pos - floor(pos);
        sample = bottomSample + (skew * difference);
        //setting up FFT arrays
        freqWaveImag[i] = (double)sample;
        freqWaveReal[i] = 0.0f;
    }
    fft(TABLESIZE, freqWaveReal, freqWaveImag);
    createTables(freqWaveReal, freqWaveImag, TABLESIZE);
    delete [] freqWaveReal;
    delete [] freqWaveImag;
}

int WavetableFrame::createTables(double *waveReal, double *waveImag, int numSamples)
{
    int idx;
    // zero DC offset and Nyquist (set first and last samples of each array to zero, in other words)
    waveReal[0] = waveImag[0] = 0.0f;
    waveReal[numSamples >> 1] = waveImag[numSamples >> 1] = 0.0f;
    // determine maxHarmonic, the highest non-zero harmonic in the wave
    int maxHarmonic = numSamples >> 1;
    const double minVal = 0.000001f;
    while((fabs(waveReal[maxHarmonic]) + fabs(waveImag[maxHarmonic]) < minVal) && maxHarmonic)
        --maxHarmonic;
    double topFreq = (double)(2.0f / 3.0f / maxHarmonic); //note:: topFreq is in units of phase fraction per sample, not Hz
    double *ar = new double [numSamples];
    double *ai = new double [numSamples];
    double scale = 0.0f;
    int numTables = 0;
    while (maxHarmonic) // cut the harmonics in half until the max is <= 0
    {
        // fill the table in with the needed harmonics
        for (idx = 0; idx < numSamples; idx++)
            ar[idx] = ai[idx] = 0.0f;
        for (idx = 1; idx <= maxHarmonic; idx++)
        {
            ar[idx] = waveReal[idx];
            ai[idx] = waveImag[idx];
            ar[numSamples - idx] = waveReal[numSamples - idx];
            ai[numSamples - idx] = waveImag[numSamples - idx];
        }
        // make the wavetable
        scale = makeTable(ar, ai, numSamples, scale, lastMinFreq, topFreq);
        numTables++;

        // prepare for next table, topFreq *= 2 because we're using 1 waveTable per octave
        lastMinFreq = topFreq;
        topFreq *= 2.0f;
        maxHarmonic >>= 1;
    }
    delete [] ar;
    delete [] ai;
    //printf("%d total wavetables\n", numTables);
    return numTables;
}
float WavetableFrame::makeTable(double *waveReal, double *waveImag, int numSamples, double scale, double bottomFreq, double topFreq)
{
    //printf("Table #%d limit: %lf\n", tablesAdded, topFreq * sampleRate);
    if(tablesAdded <= NUMTABLES)
    {
        pTables[tablesAdded].maxFreq = topFreq;
        pTables[tablesAdded].minFreq = bottomFreq;
        fft(numSamples, waveReal, waveImag);
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
            pTables[tablesAdded][i] = waveImag[i] * scale;
            if(pTables[tablesAdded][i] < minLevel)
                minLevel = pTables[tablesAdded][i];
            if(pTables[tablesAdded][i] > maxLevel)
                maxLevel = pTables[tablesAdded][i];
        }
        auto offset = maxLevel + minLevel;
        minLevel = std::numeric_limits<float>::max();
        maxLevel = std::numeric_limits<float>::min();
        for(int i = 0; i < numSamples; ++i)
        {
            pTables[tablesAdded][i] -= (offset / 2.0f); //make sure each table has no DC offset
            if(pTables[tablesAdded][i] < minLevel)
                minLevel = pTables[tablesAdded][i];
            if(pTables[tablesAdded][i] > maxLevel)
                maxLevel = pTables[tablesAdded][i];
        }
        pTables[tablesAdded].normalize(); //TODO: determine whether this is a good idea
        ++tablesAdded;
    }
    return (float)scale;
}
WaveTable* WavetableFrame::tableForFreq(double frequency)
{
    for(int i = 0; i < tablesAdded; ++i)
    {
        if(frequency < pTables[i].maxFreq && frequency <= pTables[i].minFreq)
            return &pTables[i];
    }
    return &pTables[0];
}



void WavetableFrame::clockSample(double frequency)
{
    phaseDelta = (double)(frequency / sampleRate);
    auto table = tableForFreq(phaseDelta);
    phase += phaseDelta;
    if(phase > 1.0f)
    {
        phase -= 1.0f;
    }
    bottomSampleIndex = (int)(table->length * phase);
    skew = (table->length * phase) - bottomSampleIndex;
    sampleDiff = table->table[bottomSampleIndex + 1] - table->table[bottomSampleIndex];
    output = table->table[bottomSampleIndex] + (skew * sampleDiff);
}

std::vector<float> WavetableFrame::getBasicVector(int resolution)
{
    std::vector<float> out;
    auto inc = (int)(TABLESIZE / resolution);
    for(int sample = 0; sample < resolution; ++sample)
    {
        int idx = (int)inc *sample;
        auto f = (float)pTables[0][idx]; //for purposes of graphing, always use the first table with the most harmonic detail
        out.push_back(f);
    }
    return out;
}

WavetableOsc::WavetableOsc(juce::File wavData)
{
    sampleRate = 44100.0f;
    currentPosition = 0.0f;
    numFrames = 0;
    auto manager = new juce::AudioFormatManager();
    manager->registerBasicFormats();
    auto reader = manager->createReaderFor(wavData);
    auto numSamples = reader->lengthInSamples;
    auto sNumFrames = (int)(numSamples / TABLESIZE);
    long currentSample = 0;
    auto buffer = juce::AudioBuffer<float>(1, TABLESIZE);
    buffer.clear();
    const char* str = wavData.getFileName().toRawUTF8();
    printf("File %s has %d frames\n", str, sNumFrames);
    //NOTE: this is calling the default constructor, each table's data will have to be set appropriately
    frames = new WavetableFrame[sNumFrames];
    reader->read(&buffer, 0, TABLESIZE, currentSample, true, true);
    std::array<float, TABLESIZE> fArray;
    for(int i = 0; i < sNumFrames; ++i) //assign the values from each frame
    {
        for(int sample = 0; sample < TABLESIZE; ++sample)
        {
            fArray[sample] = buffer.getSample(0, sample); //transfer each sample into fArray
        }
        frames[i] = WavetableFrame(fArray);
        ++numFrames;
        buffer.clear();
        currentSample += TABLESIZE; //increment the read position by one frame length
        reader->read(&buffer, 0, TABLESIZE, currentSample, true, true); //fill the buffer with the next frame's info
    }
    delete reader;
    delete manager;
}

std::vector<std::vector<float>> WavetableOsc::getDataToGraph(int resolution)
{
    std::vector<std::vector<float>> out;
    for(int frame = 0; frame < numFrames; ++frame)
    {
        out.push_back(frames[frame].getBasicVector(resolution));
    }
    return out;
}
