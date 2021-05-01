/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void addParameter(juce::AudioProcessorValueTreeState::ParameterLayout& layout, juce::NormalisableRange<float> range, float defaultValue, juce::String paramId, juce::String paramName)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramId, paramName, range, defaultValue));
}

juce::AudioProcessorValueTreeState::ParameterLayout createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    auto delayRange = juce::NormalisableRange<float>(DELAY_MIN, DELAY_MAX, 0.1f);
    delayRange.setSkewForCentre(DELAY_CENTER);
    auto attackRange = juce::NormalisableRange<float>(ATTACK_MIN, ATTACK_MAX, 0.1f);
    attackRange.setSkewForCentre(ATTACK_CENTER);
    auto holdRange = juce::NormalisableRange<float>(HOLD_MIN, HOLD_MAX, 0.1f);
    holdRange.setSkewForCentre(HOLD_CENTER);
    auto decayRange = juce::NormalisableRange<float>(DECAY_MIN, DECAY_MAX, 0.1f);
    decayRange.setSkewForCentre(DECAY_CENTER);
    auto sustainRange = juce::NormalisableRange<float>(SUSTAIN_MIN, SUSTAIN_MAX, 0.001f);
    auto releaseRange = juce::NormalisableRange<float>(RELEASE_MIN, RELEASE_MAX, 0.1f);
    releaseRange.setSkewForCentre(RELEASE_CENTER);
    
    auto waveRange = juce::NormalisableRange<float>(0.0f, 255.0f, 1.0f);
    auto waveId = "wavetableParam";
    auto waveName = "Current Wavetable";
    
    auto posRange = juce::NormalisableRange<float>(0.0f, 1.0f, 0.0001f);
    auto posId = "oscPositionParam";
    auto posName = "Wavetable Position";
    
    auto delayId = "delayParam";
    auto delayName = "Delay";
    auto attackId = "attackParam";
    auto attackName = "Attack";
    auto holdId = "holdParam";
    auto holdName = "Hold";
    auto decayId = "decayParam";
    auto decayName = "Decay";
    auto sustainId = "sustainParam";
    auto sustainName = "Sustain";
    auto releaseId = "releaseParam";
    auto releaseName = "Release";
    
    addParameter(layout, delayRange, DELAY_DEFAULT, delayId, delayName);
    addParameter(layout, attackRange, ATTACK_DEFAULT, attackId, attackName);
    addParameter(layout, holdRange, HOLD_DEFAULT, holdId, holdName);
    addParameter(layout, decayRange, DECAY_DEFAULT, decayId, decayName);
    addParameter(layout, sustainRange, SUSTAIN_DEFAULT, sustainId, sustainName);
    addParameter(layout, releaseRange, RELEASE_DEFAULT, releaseId, releaseName);
    addParameter(layout, waveRange, 0.0f, waveId, waveName);
    addParameter(layout, posRange, 0.0f, posId, posName);

    return layout;
}





//==============================================================================
OctaneAudioProcessor::OctaneAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
tree(*this, nullptr, "AllParameters", createLayout())
#endif
{
    
}

OctaneAudioProcessor::~OctaneAudioProcessor()
{
}

//==============================================================================
const juce::String OctaneAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OctaneAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OctaneAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OctaneAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OctaneAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OctaneAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OctaneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OctaneAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OctaneAudioProcessor::getProgramName (int index)
{
    return {};
}

void OctaneAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void OctaneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth.setAllSampleRates(sampleRate);
}

void OctaneAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OctaneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void OctaneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool OctaneAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OctaneAudioProcessor::createEditor()
{
    return new OctaneAudioProcessorEditor(*this);
}

//==============================================================================
void OctaneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void OctaneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OctaneAudioProcessor();
}
