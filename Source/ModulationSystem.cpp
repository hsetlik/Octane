/*
  ==============================================================================

    ModulationSystem.cpp
    Created: 1 May 2021 11:02:04am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "ModulationSystem.h"



float SynthParam::getAdjusted(int voice)
{
    return (max - min) * getActual(voice);
}

ModSource* SynthParam::makeSource(float d)
{
    return new ModSource(this, d);
}

float SynthParam::actualOffset(ModSource *mod, int index)
{
    return (max - min) * (mod->src->getAdjusted(index) * mod->depth);
}

void SynthParam::removeSource(ModSource *toRemove)
{
    int idx = 0;
    for(auto src : modSources)
    {
        if(src == toRemove)
        {
            modSources.erase(modSources.begin() + idx);
            break;
        }
        ++idx;
    }
}

void SynthParam::tickValue()
{
    if(currentBaseValue == targetBaseValue)
    {
        return;
    }
    else if(targetBaseValue > (currentBaseValue + maxSampleDelta))
    {
        currentBaseValue += maxSampleDelta;
    }
    else if(targetBaseValue < (currentBaseValue - maxSampleDelta))
    {
        currentBaseValue -= maxSampleDelta;
    }
    else
    {
        currentBaseValue = targetBaseValue;
    }
}

//=======================================================================

SynthParameterGroup::SynthParameterGroup(juce::AudioProcessorValueTreeState* tree) : //this constructor does the heavy lifting of creating parameters
filterCutoff("FilterCutoff", CUTOFF_MIN, CUTOFF_MAX, CUTOFF_DEFAULT),
filterResonance("FilterResonance", RESONANCE_MIN, RESONANCE_MAX, RESONANCE_DEFAULT),
filterWetDry("FilterWetDry", 0.0f, 1.0f, 1.0f),
pitchWheelValue("PitchWheel", -1.0f, 1.0f, 0.0f),
modWheelValue("ModWheel", 0.0f, 1.0f, 0.0f),
keyTrackValue("KeyTrack", 0.0f, 1.0f, 0.0f),
lastTriggeredVoice("LastVoice", 0.0f, (float)NUM_VOICES + 1, 0.0f),
linkedTree(tree)
{
    for(int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        auto iStr = juce::String(i);
        juce::String posName = "OscillatorPos" + iStr;
        oscPositions.add(new VoiceTargetParam(posName, 0.0f, 1.0f, 0.0f));
        auto levelName = "OscillatorLevel" + iStr;
        oscLevels.add(new VoiceTargetParam(levelName, 0.0f, 1.0f, 1.0f));
        auto panName = "OscillatorPan" + iStr;
        oscPans.add(new VoiceTargetParam(panName, 0.0f, 1.0f, 0.5f));
        auto powerName = "OscillatorPower" + iStr;
        oscPowers.add(new MacroSourceParam(powerName, 0.0f, 1.0f, 1.0f));
        auto uModeName = "OscillatorUnisonMode" + iStr;
        oscUnisonModes.add(new MacroSourceParam(uModeName, 0.0f, 1.0f, 0.0f));
        auto uSpreadName = "OscillatorUnisonSpread" + iStr;
        oscUnisonSpreads.add(new GlobalTargetParam(uSpreadName, 0.0f, 1.0f, 0.5f));
        auto uVoicesName = "OscillatorUnisonVoices" + iStr;
        oscUnisonVoices.add(new GlobalTargetParam(uVoicesName, 0.0f, (float)UNISON_MAX, 1.0f));
        auto uLevelName = "OscillatorUnisonLevel" + iStr;
        oscUnisonLevels.add(new GlobalTargetParam(uLevelName, 0.0f, 1.0f, 0.0f));
        //each oscillator gets an amp envelope and a mod envelope
        auto mDelay = "ModEnvDelay" + iStr;
        auto aDelay = "AmpEnvDelay" + iStr;
        auto mAttack = "ModEnvAttack" + iStr;
        auto aAttack = "AmpEnvAttack" + iStr;
        auto mHold = "ModEnvHold" + iStr;
        auto aHold = "AmpEnvHold" + iStr;
        auto mDecay = "ModEnvDecay" + iStr;
        auto aDecay = "AmpEnvDecay" + iStr;
        auto mSustain = "ModEnvSustain" + iStr;
        auto aSustain = "AmpEnvSustain" + iStr;
        auto mRelease = "ModEnvRelease" + iStr;
        auto aRelease = "AmpEnvRelease" + iStr;
        
        mDelays.add(new GlobalTargetParam(mDelay, DELAY_MIN, DELAY_MAX, DELAY_DEFAULT));
        aDelays.add(new GlobalTargetParam(aDelay, DELAY_MIN, DELAY_MAX, DELAY_DEFAULT));
        mAttacks.add(new GlobalTargetParam(mAttack, ATTACK_MIN, ATTACK_MAX, ATTACK_DEFAULT));
        aAttacks.add(new GlobalTargetParam(aAttack, ATTACK_MIN, ATTACK_MAX, ATTACK_DEFAULT));
        mHolds.add(new GlobalTargetParam(mHold, HOLD_MIN, HOLD_MAX, HOLD_DEFAULT));
        aHolds.add(new GlobalTargetParam(aHold, HOLD_MIN, HOLD_MAX, HOLD_DEFAULT));
        mDecays.add(new GlobalTargetParam(mDecay, DECAY_MIN, DECAY_MAX, DECAY_DEFAULT));
        aDecays.add(new GlobalTargetParam(aDecay, DECAY_MIN, DECAY_MAX, DECAY_DEFAULT));
        mSustains.add(new GlobalTargetParam(mSustain, SUSTAIN_MIN, SUSTAIN_MAX, SUSTAIN_DEFAULT));
        aSustains.add(new GlobalTargetParam(aSustain, SUSTAIN_MIN, SUSTAIN_MAX, SUSTAIN_DEFAULT));
        mReleases.add(new GlobalTargetParam(mRelease, RELEASE_MIN, RELEASE_MAX, RELEASE_DEFAULT));
        aReleases.add(new GlobalTargetParam(aRelease, RELEASE_MIN, RELEASE_MAX, RELEASE_DEFAULT));
        
        auto mOutput = "ModEnvOutput" + iStr;
        auto aOutput = "AmpEnvOutput" + iStr;
        
        oscModEnvs.add(new ContinuousVoiceParam(mOutput, 0.0f, 1.0f, 0.0f));
        oscAmpEnvs.add(new ContinuousVoiceParam(aOutput, 0.0f, 1.0f, 0.0f));
    }
    for(int i = 0; i < NUM_LFOS; ++i)
    {
        auto iStr = juce::String(i);
        auto lfoId = "LfoOutput" + iStr;
        lfoOutputs.add( new ContinuousVoiceParam(lfoId, 0.0f, 1.0f, 0.0f));
        
        auto rateId = "LfoRate" + iStr;
        auto retrigId = "LfoRetrig" + iStr;
        lfoRates.add(new GlobalTargetParam(rateId, RATE_MIN, RATE_MAX, RATE_DEFAULT));
        lfoRetriggers.add(new GlobalTargetParam(retrigId, 0.0f, 1.0f, 0.0f));
        auto powerId = "LfoPower" + iStr;
        lfoPowers.add(new MacroSourceParam(powerId, 0.0f, 1.0f, 1.0f));
    }
}

void SynthParameterGroup::updateForBlock(apvts &tree)
{
    
}

juce::AudioProcessorValueTreeState::ParameterLayout SynthParameterGroup::createLayout()
{
    apvts::ParameterLayout layout;
    auto delayRange = fRange(DELAY_MIN, DELAY_MAX, 0.1f);
    delayRange.setSkewForCentre(DELAY_CENTER);
    auto attackRange = fRange(ATTACK_MIN, ATTACK_MAX, 0.1f);
    attackRange.setSkewForCentre(ATTACK_CENTER);
    auto holdRange = fRange(HOLD_MIN, HOLD_MAX, 0.1f);
    holdRange.setSkewForCentre(HOLD_CENTER);
    auto decayRange = fRange(DECAY_MIN, DECAY_MAX, 0.1f);
    decayRange.setSkewForCentre(DECAY_CENTER);
    auto sustainRange = fRange(SUSTAIN_MIN, SUSTAIN_MAX, 0.0001f);
    auto releaseRange = fRange(RELEASE_MIN, RELEASE_MAX, 0.1f);
    releaseRange.setSkewForCentre(RELEASE_CENTER);
    auto posRange = fRange(0.0f, 1.0f, 0.0001f);
    auto levelRange = fRange(0.0f, 1.0f, 0.0001f);
    for(int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        auto iStr = juce::String(i);
        auto mDelay = "ModEnvDelay" + iStr;
        auto aDelay = "AmpEnvDelay" + iStr;
        auto mAttack = "ModEnvAttack" + iStr;
        auto aAttack = "AmpEnvAttack" + iStr;
        auto mHold = "ModEnvHold" + iStr;
        auto aHold = "AmpEnvHold" + iStr;
        auto mDecay = "ModEnvDecay" + iStr;
        auto aDecay = "AmpEnvDecay" + iStr;
        auto mSustain = "ModEnvSustain" + iStr;
        auto aSustain = "AmpEnvSustain" + iStr;
        auto mRelease = "ModEnvRelease" + iStr;
        auto aRelease = "AmpEnvRelease" + iStr;
        
        auto posId = "OscillatorPos" + iStr;
        auto levelId = "OscillatorLevel" + iStr;
        auto panId = "OscillatorPan" + iStr;
        
        layout.add(std::make_unique<floatParam>(mDelay, mDelay, delayRange, DELAY_DEFAULT));
        layout.add(std::make_unique<floatParam>(aDelay, aDelay, delayRange, DELAY_DEFAULT));
        layout.add(std::make_unique<floatParam>(mAttack, mAttack, attackRange, ATTACK_DEFAULT));
        layout.add(std::make_unique<floatParam>(aAttack, aAttack, attackRange, ATTACK_DEFAULT));
        layout.add(std::make_unique<floatParam>(mHold, mHold, holdRange, HOLD_DEFAULT));
        layout.add(std::make_unique<floatParam>(aHold, aHold, holdRange, HOLD_DEFAULT));
        layout.add(std::make_unique<floatParam>(mDecay, mDecay, decayRange, DECAY_DEFAULT));
        layout.add(std::make_unique<floatParam>(aDecay, aDecay, decayRange, DECAY_DEFAULT));
        layout.add(std::make_unique<floatParam>(mSustain, mSustain, sustainRange, SUSTAIN_DEFAULT));
        layout.add(std::make_unique<floatParam>(aSustain, aSustain, sustainRange, SUSTAIN_DEFAULT));
        layout.add(std::make_unique<floatParam>(mRelease, mRelease, releaseRange, RELEASE_DEFAULT));
        layout.add(std::make_unique<floatParam>(aRelease, aRelease, releaseRange, RELEASE_DEFAULT));
        
        layout.add(std::make_unique<floatParam>(posId, posId, posRange, 0.0f));
        layout.add(std::make_unique<floatParam>(levelId, levelId, levelRange, 1.0f));
        layout.add(std::make_unique<floatParam>(panId, panId, levelRange, 0.5f));
    }
    for(int i = 0; i < NUM_LFOS; ++i)
    {
        auto iStr = juce::String(i);
        auto rateRange = fRange(RATE_MIN, RATE_MAX, 0.0001f);
        rateRange.setSkewForCentre(RATE_CENTER);
        auto retrigRange = fRange(0.0f, 1.0f, 0.5f);
        auto rateId = "LfoRate" + iStr;
        auto retrigId = "LfoRetrig" + iStr;
        layout.add(std::make_unique<floatParam>(rateId, rateId, rateRange, RATE_DEFAULT));
        layout.add(std::make_unique<floatParam>(retrigId, retrigId, retrigRange, 0.0f));
    }
    return layout;
}

