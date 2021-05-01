/*
  ==============================================================================

    Synthesizer.cpp
    Created: 30 Apr 2021 3:59:28pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Synthesizer.h"
OctaneVoice::OctaneVoice(juce::File defaultTable, SynthParameterGroup* grp, int idx) :
params(grp),
voiceIndex(idx),
fundamental(440.0f),
osc1(defaultTable)
{
    
}

OctaneVoice::~OctaneVoice()
{
   
}

void OctaneVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    osc1.triggerOn();
    fundamental = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
}

void OctaneVoice::stopNote(float velocity, bool allowTailOff)
{
    osc1.triggerOff();
    if(!allowTailOff || !osc1.ampEnv.isActive())
        clearCurrentNote();
}

void OctaneVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    if(!osc1.ampEnv.isActive())
        clearCurrentNote();
    for(sample = startSample; sample < (startSample + numSamples); ++sample)
    {
        lastOutput = osc1.getSample(fundamental);
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
    
}
//==============================================================================================================

OctaneSynth::OctaneSynth()
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
        addVoice(new OctaneVoice(defaultWave, &paramGroup, i));
        auto vOct = dynamic_cast<OctaneVoice*>(voices.getLast());
        oVoices.push_back(vOct);
    }
    addSound(new OctaneSound());
}
