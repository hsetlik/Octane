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
        lfos.add(new OctaneLFO());
        auto pOut = params->lfoOutputs[lfoIndex];
        lfoOutputs.push_back(pOut);
    }
    
}

OctaneVoice::~OctaneVoice()
{
   
}

void OctaneVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    params->keyTrackValue.setOutput(voiceIndex, (float)(midiNoteNumber / MIDI_NOTES));
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

void OctaneVoice::pitchWheelMoved(int newPitchWheelVal)
{
    auto scaledVal = (float)newPitchWheelVal / (float)(2 * PITCHWHEEL_RANGE); //! remap to the range 0-2
    params->pitchWheelValue.setBase(scaledVal - 1.0f); //! remap to -1 - 1 range
}

void OctaneVoice::controllerMoved(int controllerNumber, int controllerValue)
{
    if(controllerNumber == 1)
    {
        params->modWheelValue.setBase((float)(controllerValue / PITCHWHEEL_RANGE));
    }
}

void OctaneVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    tickBlock();
    for(sample = startSample; sample < (startSample + numSamples); ++sample)
    {
        tickSample();
        outputBuffer.addSample(0, sample, lastOutL * 0.5f);
        outputBuffer.addSample(1, sample, lastOutR * 0.5f);
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
        lfos[oscIndex]->setTrigger((params->lfoRetriggers[oscIndex]->getActual() > 0.0f));
    }
}

void OctaneVoice::tickSample()
{
    lastOutput = 0.0f;
    lastOutL = 0.0f;
    lastOutR = 0.0f;
    lastOscLevel = 0.0f;
    oscLevelSum = 0.5f;
    filter.setCutoff(params->filterCutoff.getActual(voiceIndex));
    filter.setResonance(params->filterResonance.getActual(voiceIndex));
    filter.setWetDry(params->filterWetDry.getActual(voiceIndex));
    for(oscIndex = 0; oscIndex < NUM_OSCILLATORS; ++oscIndex)
    {
        lfoOutputs[oscIndex]->setOutput(voiceIndex, lfos[oscIndex]->getOutput());
        ampOutputs[oscIndex]->setOutput(voiceIndex, oscillators[oscIndex]->lastAmpEnv());
        modOutputs[oscIndex]->setOutput(voiceIndex, oscillators[oscIndex]->lastModEnv());
        oscillators[oscIndex]->setPosition(params->oscPositions[oscIndex]->getActual(voiceIndex));
        lastOscLevel = params->oscLevels[oscIndex]->getActual(voiceIndex);
        oscPanValues[oscIndex] = params->oscPans[oscIndex]->getActual(voiceIndex);
        oscLevelSum += lastOscLevel;
        oscillators[oscIndex]->setLevel(lastOscLevel);
        lastOutput += (oscillators[oscIndex]->getSample(fundamental) / oscLevelSum);
        lastOutL += (1.0f - oscPanValues[oscIndex]) * lastOutput;
        lastOutR += oscPanValues[oscIndex] * lastOutput;
    }
    lastOutR = filter.processR(lastOutR);
    lastOutL = filter.processL(lastOutL);
}

void OctaneVoice::replaceWave(int index, juce::File newWave)
{
    oscillators[index]->replace(newWave);
}

std::vector<juce::File> OctaneVoice::getOscFiles()
{
    std::vector<juce::File> out;
    for(auto osc : oscillators)
    {
        out.push_back(osc->getSource());
    }
    return out;
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
    activeFiles = oVoices[0]->getOscFiles();
}

void OctaneSynth::replaceLfos(int index)
{
    auto& array = paramGroup.lfoShapes[index];
    for(auto voice : oVoices)
    {
        voice->lfos[index]->resetFromArray(array);
    }
}

void OctaneSynth::replaceWaves(int index, juce::File newWave)
{
    for(auto voice : oVoices)
    {
        voice->replaceWave(index, newWave);
    }
    activeFiles = oVoices[0]->getOscFiles();
}
//==========================================================================
OctaneLFOChange::OctaneLFOChange(OctaneSynth* synth, int lfoIndex) :
linkedSynth(synth),
index(lfoIndex)
{
}
void OctaneLFOChange::apply()
{
    linkedSynth->replaceLfos(index);
}
//===========================================================================
OctaneWaveChange::OctaneWaveChange(OctaneSynth* synth, int oscIndex, juce::File newWave) :
linkedSynth(synth),
index(oscIndex),
wave(newWave)
{
    
}
void OctaneWaveChange::apply()
{
    linkedSynth->replaceWaves(index, wave);
    linkedSynth->updateGraphData();
}
//===========================================================================
OctaneUpdater::OctaneUpdater(OctaneSynth* synth) : linkedSynth(synth), blockIndex(0)
{
    
}
void OctaneUpdater::tick()
//! this gets called in \c processBlock() to run the needed updates once every several buffers
//! note that smaller buffer size means more frequent updates
{
    ++blockIndex;
    if(blockIndex == BLOCKS_PER_UPDATE)
    {
        triggerAsyncUpdate();
        blockIndex = 0;
    }
}
void OctaneUpdater::stageLfoChange(int index)
{
    stagedChanges.add(new OctaneLFOChange(linkedSynth, index));
}
void OctaneUpdater::stageWaveChange(int index, juce::File wave)
{
    stagedChanges.add(new OctaneWaveChange(linkedSynth, index, wave));
}
void OctaneUpdater::handleAsyncUpdate()
{
    for(auto change : stagedChanges)
        change->apply();
    stagedChanges.clear();
}
