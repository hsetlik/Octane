/*
  ==============================================================================

    StkUtil.h
    Created: 14 May 2021 12:00:46pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace StkUtil
{
    using namespace stk;
    void loadFramesFromBuffer(StkFrames& frames, juce::AudioBuffer<float>& buffer)
    {
        //! only resize if the sizes are different
        int sample, channel;
        if((int)frames.size() != buffer.getNumSamples() || frames.channels() != buffer.getNumChannels())
        {
            frames.resize((size_t)buffer.getNumSamples(), buffer.getNumChannels());
        }
        for(sample = 0; sample < (int)frames.size(); ++sample)
        {
            for(channel = 0; channel < frames.channels(); ++channel)
            {
                frames((size_t)sample, channel) = buffer.getSample(channel, sample);
            }
        }
    }
    void loadBufferFromFrames(StkFrames& frames, juce::AudioBuffer<float>& buffer)
    {
        int sample, channel;
        if((int)frames.size() != buffer.getNumSamples() || frames.channels() != buffer.getNumChannels())
        {
            buffer.setSize((int)frames.channels(), (int)frames.size(), false, true, true);
        }
        for(sample = 0; sample < (int)frames.size(); ++sample)
        {
            for(channel = 0; channel < frames.channels(); ++channel)
            {
                buffer.setSample(channel, sample, frames((size_t)sample, channel));
            }
        }
    }
};
