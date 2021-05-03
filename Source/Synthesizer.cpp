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
    }
}

OctaneVoice::~OctaneVoice()
{
   
}

void OctaneVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    for(auto o : oscillators)
        o->triggerOn();
    fundamental = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
}

void OctaneVoice::stopNote(float velocity, bool allowTailOff)
{
    for(auto o : oscillators)
        o->triggerOff();
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
    
}

void OctaneVoice::tickSample()
{
    lastOutput = 0.0f;
    for(oscIndex = 0; oscIndex < NUM_OSCILLATORS; ++oscIndex)
    {
        oscillators[oscIndex]->setPosition(params->oscPositions[oscIndex]->getActual());
        oscillators[oscIndex]->setLevel(params->oscLevels[oscIndex]->getActual());
        lastOutput += (oscillators[oscIndex]->getSample(fundamental) / (float) NUM_OSCILLATORS);
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
}
