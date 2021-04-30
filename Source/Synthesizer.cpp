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
    
}

void OctaneVoice::stopNote(float velocity, bool allowTailOff)
{
    
}

void OctaneVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    
}


OctaneSynth::OctaneSynth()
{
    
}
