/*
  ==============================================================================

    Synthesizer.cpp
    Created: 30 Apr 2021 3:59:28pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Synthesizer.h"
OctaneVoice::OctaneVoice(juce::Array<juce::File>& waveFolder, SynthParameterGroup* grp, int idx) :
params(grp),
voiceIndex(idx),
fundamental(440.0f)
{
    for(int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        oscillators.add(new OctaneOsc(waveFolder[i + 1]));
        auto pAmp = params->oscAmpEnvs[i];
        auto pMod = params->oscModEnvs[i];
        ampOutputs.push_back(pAmp);
        modOutputs.push_back(pMod);
    }
    for(lfoIndex = 0; lfoIndex < NUM_LFOS; ++lfoIndex)
    {
        lfos.add(new OctaneLFO(LFO_Functions::createSineTable));
        auto pOut = params->lfoOutputs[lfoIndex];
        lfoOutputs.push_back(pOut);
    }
    
}

OctaneVoice::~OctaneVoice()
{
   
}

void OctaneVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    for(auto o : oscillators)
        o->triggerOn();
    for(auto l : lfos)
        l->noteOn();
    fundamental = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
}

void OctaneVoice::stopNote(float velocity, bool allowTailOff)
{
    for(auto o : oscillators)
        o->triggerOff();
    for(auto l : lfos)
        l->noteOff();
    if(!allowTailOff)
        clearCurrentNote();
}

void OctaneVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    tickBlock();
    for(sample = startSample; sample < (startSample + numSamples); ++sample)
    {
        tickSample();
        for(auto chan = 0; chan < outputBuffer.getNumChannels(); ++chan)
        {
            outputBuffer.addSample(chan, sample, lastOutput * 0.4f);
        }
    }
}

void OctaneVoice::tickBlock()
{
    for(oscIndex = 0; oscIndex < NUM_OSCILLATORS; ++oscIndex)
    {
        oscillators[oscIndex]->ampEnv.setDelay(params->aDelays[oscIndex]->getActual());
        oscillators[oscIndex]->modEnv.setDelay(params->mDelays[oscIndex]->getActual());
        oscillators[oscIndex]->ampEnv.setAttack(params->aAttacks[oscIndex]->getActual());
        oscillators[oscIndex]->modEnv.setAttack(params->mAttacks[oscIndex]->getActual());
        oscillators[oscIndex]->ampEnv.setHold(params->aHolds[oscIndex]->getActual());
        oscillators[oscIndex]->modEnv.setHold(params->mHolds[oscIndex]->getActual());
        oscillators[oscIndex]->ampEnv.setDecay(params->aDecays[oscIndex]->getActual());
        oscillators[oscIndex]->modEnv.setDecay(params->mDecays[oscIndex]->getActual());
        oscillators[oscIndex]->ampEnv.setSustain(params->aSustains[oscIndex]->getActual());
        oscillators[oscIndex]->modEnv.setSustain(params->mSustains[oscIndex]->getActual());
        oscillators[oscIndex]->ampEnv.setRelease(params->aReleases[oscIndex]->getActual());
        oscillators[oscIndex]->modEnv.setRelease(params->mReleases[oscIndex]->getActual());
        lfos[oscIndex]->setRateHz(params->lfoRates[oscIndex]->getActual());
        lfos[oscIndex]->setTrigger((params->lfoRetriggers[oscIndex]->getActual() > 0.0f) ? true : false);
    }
}

void OctaneVoice::tickSample()
{
    lastOutput = 0.0f;
    lastOscLevel = 0.0f;
    oscLevelSum = 1.0f;
    for(oscIndex = 0; oscIndex < NUM_OSCILLATORS; ++oscIndex)
    {
        lfoOutputs[oscIndex]->setOutput(voiceIndex, lfos[oscIndex]->getOutput());
        ampOutputs[oscIndex]->setOutput(voiceIndex, oscillators[oscIndex]->lastAmpEnv());
        modOutputs[oscIndex]->setOutput(voiceIndex, oscillators[oscIndex]->lastModEnv());
        oscillators[oscIndex]->setPosition(params->oscPositions[oscIndex]->getActual(voiceIndex));
        lastOscLevel = params->oscLevels[oscIndex]->getActual(voiceIndex);
        oscLevelSum += lastOscLevel;
        oscillators[oscIndex]->setLevel(lastOscLevel);
        lastOutput += (oscillators[oscIndex]->getSample(fundamental) / oscLevelSum);
    }
}
//==============================================================================================================

OctaneSynth::OctaneSynth(juce::AudioProcessorValueTreeState* tree) :
paramGroup(tree)
{
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
    //printf("Folder is at: %s\n", waveFolder.getFullPathName().toRawUTF8());
    waveFiles = waveFolder.findChildFiles(juce::File::findFiles, true);
    auto defaultWave = waveFiles[0];
    for(int i = 0; i < NUM_VOICES; ++i)
    {
        addVoice(new OctaneVoice(waveFiles, &paramGroup, i));
        auto vOct = dynamic_cast<OctaneVoice*>(voices.getLast());
        oVoices.push_back(vOct);
    }
    addSound(new OctaneSound());
    int idx = 0;
    for(auto& osc : paramGroup.oscGraphVectors)
    {
        osc = oVoices[0]->oscillators[idx]->getGraphData(128);
        ++idx;
    }
}
