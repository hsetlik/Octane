/*
  ==============================================================================

    PatchBrowser.h
    Created: 25 May 2021 8:54:11am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "RgbColor.h"
#include "ModulationSystem.h"
#include "SymbolButton.h"
#include "Synthesizer.h"
#include "GraphicsUtility.h"

class OctaneBrowser : public juce::Component
{
public:
    OctaneBrowser(OctaneUpdater* updater);
    void loadPreset(int index);
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    OctaneUpdater* const linkedUpdater;
    juce::Array<juce::File> patchFiles;
    juce::File patchFolder;
    OctaneButtons::Text dropButton;
    
    
};
