/*
  ==============================================================================

    AudioFileLoader.h
    Created: 7 Mar 2021 9:52:48pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
class AudioWavetableHandler
{
public:
    AudioWavetableHandler();
    ~AudioWavetableHandler() {}
    juce::String nameAtIndex(int index)
    {
        return tableNames[index];
    }
    juce::File getWav(int index)
    {
        return wavFiles[index];
    }
    juce::File getWav(juce::String name)
    {
        printf("Handler asked for: '%s'\n", name.toRawUTF8());
        return wavFiles[tableNames.indexOf(name)];
    }
    static int getNumWavetables();
    juce::StringArray tableNames;
    juce::Array<juce::File> wavFiles;
private:
    int numFiles;
    juce::File audioFolder;
    
};
