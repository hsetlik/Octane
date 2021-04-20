/*
  ==============================================================================

    Synthesizer.cpp
    Created: 19 Apr 2021 4:21:38pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Synthesizer.h"

WavetableVoice::WavetableVoice(juce::File& file) : osc(file)
{
    pDelay.setRange(DELAY_MIN, DELAY_MAX);
    pDelay.set(DELAY_DEFAULT);
    pDelay.setTreeString("delayParam");
    pAttack.setRange(ATTACK_MIN, ATTACK_MAX);
    pAttack.set(ATTACK_DEFAULT);
    pAttack.setTreeString("attackParam");
    pHold.setRange(HOLD_MIN, HOLD_MAX);
    pHold.set(HOLD_DEFAULT);
    pHold.setTreeString("holdParam");
    pDecay.setRange(DECAY_MIN, DECAY_MAX);
    pDecay.set(DECAY_DEFAULT);
    pDecay.setTreeString("decayParam");
    pSustain.setRange(SUSTAIN_MIN, SUSTAIN_MAX);
    pSustain.set(SUSTAIN_DEFAULT);
    pSustain.setTreeString("sustainParam");
    pRelease.setRange(RELEASE_MIN, RELEASE_MAX);
    pRelease.set(RELEASE_DEFAULT);
    pRelease.setTreeString("releaseParam");
}

void WavetableVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    fundamental = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    env.triggerOn();
}

void WavetableVoice::stopNote(float velocity, bool allowTailOff)
{
    env.triggerOff();
}

void WavetableVoice::setSampleRate(double newRate)
{
    osc.setSampleRate(newRate);
    env.setSampleRate(newRate);
    setCurrentPlaybackSampleRate(newRate);
}

void WavetableVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    outputBuffer.clear();
    for(int i = startSample; i < (startSample + numSamples); ++i)
    {
        updateEnvelope();
        lastOutput = env.process(osc.getSample(fundamental));
        for(int chan = 0; chan < outputBuffer.getNumChannels(); ++chan)
        {
            outputBuffer.addSample(chan, i, lastOutput * 0.6f);
        }
    }
}

void WavetableVoice::updateParams(juce::AudioProcessorValueTreeState *t)
{
    pDelay.updateFromTree(t);
    pAttack.updateFromTree(t);
    pHold.updateFromTree(t);
    pSustain.updateFromTree(t);
    pRelease.updateFromTree(t);
    pDecay.updateFromTree(t);
}

void WavetableVoice::updateEnvelope()
{
    env.setDelay(pDelay.get());
    env.setAttack(pAttack.get());
    env.setHold(pHold.get());
    env.setDecay(pDecay.get());
    env.setSustain(pSustain.get());
    env.setRelease(pRelease.get());
}


WavetableSynth::WavetableSynth(juce::AudioProcessorValueTreeState* t) : tree(t)
{
    lastFileIdx = 0;
    auto appFolder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    appFolder.setAsCurrentWorkingDirectory();
    waveFolder = appFolder.getChildFile("MyWavetables");
    if(waveFolder.exists() && waveFolder.isDirectory())
    {
        printf("wave folder exists\n");
        waveFolder.setAsCurrentWorkingDirectory();
    }
    else
    {
        waveFolder.createDirectory();
        waveFolder.setAsCurrentWorkingDirectory();
        printf("wave folder created\n");
    }
    printf("Folder is at: %s\n", waveFolder.getFullPathName().toRawUTF8());
    auto files = waveFolder.findChildFiles(juce::File::findFiles, true);
    auto defaultFile = files[0];
    lastOscFile = defaultFile;
    for(int i = 0; i < NUM_VOICES; ++i)
    {
        addVoice(new WavetableVoice(defaultFile));
        //! the \c WTvoices vector is just an easier way to access \c WavetableVoice members without having to dynamic cast every time
        auto* wtVoice = dynamic_cast<WavetableVoice*>(voices.getLast());
        WTvoices.push_back(wtVoice);
    }
}

void WavetableSynth::replaceOscillators(juce::File newFile)
{
    lastOscFile = newFile;
    for(auto voice : WTvoices)
    {
        voice->osc.replaceFromFile(newFile);
    }
        
}

void WavetableSynth::setSampleRate(double newRate)
{
    setCurrentPlaybackSampleRate(newRate);
    for(auto voice : WTvoices)
        voice->setSampleRate(newRate);
}

void WavetableSynth::updateFromTree()
{
    for(auto voice : WTvoices)
    {
        voice->updateParams(tree);
    }
}

std::vector<std::vector<float>> WavetableSynth::getDataToGraph()
{
    return WTvoices[0]->osc.getDataToGraph(128);
}
