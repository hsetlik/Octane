/*
  ==============================================================================

    PatchBrowser.cpp
    Created: 25 May 2021 8:54:11am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "PatchBrowser.h"

OctaneBrowser::OctaneBrowser(OctaneUpdater* const updater) : linkedUpdater(updater)
{
    
    auto appFolder = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    appFolder.setAsCurrentWorkingDirectory();
    patchFolder = appFolder.getChildFile("Octane_Patches");
    if(patchFolder.exists() && patchFolder.isDirectory())
    {
        printf("patch folder exists\n");
        patchFolder.setAsCurrentWorkingDirectory();
    }
    else
    {
        patchFolder.createDirectory();
        patchFolder.setAsCurrentWorkingDirectory();
        printf("patch folder created\n");
    }
    patchFolder.findChildFiles(patchFiles, juce::File::findFiles, true);
    addAndMakeVisible(&dropButton);
    dropButton.onClick = [&]
    {
        juce::PopupMenu menu;
        for(int i = 0; i < patchFiles.size(); ++i)
        {
            menu.addItem(patchFiles[i].getFileName(), [this, i](){this->loadPreset(i);});
            if(i % 10 == 0)
                menu.addColumnBreak();
        }
        menu.showMenuAsync(juce::PopupMenu::Options{}.withTargetComponent(&dropButton));
    };
    dropButton.setButtonText("dummy text");
}

void OctaneBrowser::loadPreset(int index)
{
    printf("Loading Preset: %d\n", index);
}

void OctaneBrowser::resized()
{
    auto dX = getWidth() / 15;
    auto dY = getHeight() / 15;
    dropButton.setBounds(dX, 6 * dY, 8 * dX, 2 * dY);
}
void OctaneBrowser::paint(juce::Graphics &g)
{
    
}
