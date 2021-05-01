/*
  ==============================================================================

    Synthesizer.cpp
    Created: 30 Apr 2021 3:59:28pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Synthesizer.h"
OctaneVoice::OctaneVoice(juce::File defaultTable) : osc(defaultTable)
{
    
}

void OctaneVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    env.triggerOn();
}

void OctaneVoice::stopNote(float velocity, bool allowTailOff)
{
    env.triggerOff();
}

void OctaneVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    
}


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
    auto files = waveFolder.findChildFiles(juce::File::findFiles, true);
    auto defaultWave = files[0];
    for(int i = 0; i < NUM_VOICES; ++i)
    {
        addVoice(new OctaneVoice(defaultWave));
    }
    addSound(new OctaneSound());
}
