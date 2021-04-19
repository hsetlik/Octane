/*
  ==============================================================================

    AudioFileLoader.cpp
    Created: 7 Mar 2021 9:52:48pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "AudioFileLoader.h"


AudioWavetableHandler::AudioWavetableHandler()
{
    auto appFolder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    appFolder.setAsCurrentWorkingDirectory();
    audioFolder = appFolder.getChildFile("MyWavetables");
    if(audioFolder.exists() && audioFolder.isDirectory())
    {
        printf("wave folder exists\n");
        audioFolder.setAsCurrentWorkingDirectory();
    }
    else
    {
        audioFolder.createDirectory();
        audioFolder.setAsCurrentWorkingDirectory();
        printf("wave folder created\n");
    }
    printf("Folder is at: %s\n", audioFolder.getFullPathName().toRawUTF8());
    auto files = audioFolder.findChildFiles(juce::File::findFiles, true);
    numFiles = 0;
    if(files.size() > 0)
    {
        for(auto i : files)
        {
            auto name = i.getFileNameWithoutExtension();
            auto extension = i.getFileExtension();
            if(extension == ".wav" || extension == ".WAV")
            {
                wavFiles.add(i);
                tableNames.add(name);
                printf(".wav found: %s\n", i.getFileName().toRawUTF8());
            }
        }
    }
}

int AudioWavetableHandler::getNumWavetables()
{
    auto appFolder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    appFolder.setAsCurrentWorkingDirectory();
    auto waveFolder = appFolder.getChildFile("MyWavetables");
    auto files = waveFolder.findChildFiles(juce::File::findFiles, true);
    return files.size();
}
