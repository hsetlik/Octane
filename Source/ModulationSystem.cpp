/*
  ==============================================================================

    ModulationSystem.cpp
    Created: 1 May 2021 11:02:04am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "ModulationSystem.h"

float SynthParam::getActual()
{
    actualOut = baseValue;
    for(auto src : modSources)
    {
        actualOut += actualOffset(src);
    }
    return actualOut;
}

float SynthParam::getAdjusted()
{
    return (max - min) * getActual();
}

ModSource* SynthParam::makeSource(float d)
{
    return new ModSource(this, d);
}

float SynthParam::actualOffset(ModSource *mod)
{
    return (max - min) * (mod->src->getAdjusted() * mod->depth);
}

//=======================================================================

SynthParameterGroup::SynthParameterGroup() //this constructor does the heavy lifting of creating parameters
{
    for(int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        auto iStr = juce::String(i);
        juce::String posName = "OscillatorPos" + iStr;
        oscPositions.add(new SynthParam(posName, 0.0f, 1.0f, 0.0f));
        auto levelName = "OscillatorLevel" + iStr;
        oscLevels.add(new SynthParam(levelName, 0.0f, 1.0f, 1.0f));
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
        
        mDelays.add(new SynthParam(mDelay, DELAY_MIN, DELAY_MAX, DELAY_DEFAULT));
        aDelays.add(new SynthParam(aDelay, DELAY_MIN, DELAY_MAX, DELAY_DEFAULT));
        mAttacks.add(new SynthParam(mAttack, ATTACK_MIN, ATTACK_MAX, ATTACK_DEFAULT));
        aAttacks.add(new SynthParam(aAttack, ATTACK_MIN, ATTACK_MAX, ATTACK_DEFAULT));
        mHolds.add(new SynthParam(mHold, HOLD_MIN, HOLD_MAX, HOLD_DEFAULT));
        aHolds.add(new SynthParam(aHold, HOLD_MIN, HOLD_MAX, HOLD_DEFAULT));
        mDecays.add(new SynthParam(mDecay, DECAY_MIN, DECAY_MAX, DECAY_DEFAULT));
        aDecays.add(new SynthParam(aDecay, DECAY_MIN, DECAY_MAX, DECAY_DEFAULT));
        mSustains.add(new SynthParam(mSustain, SUSTAIN_MIN, SUSTAIN_MAX, SUSTAIN_DEFAULT));
        aSustains.add(new SynthParam(aDelay, SUSTAIN_MIN, SUSTAIN_MAX, SUSTAIN_DEFAULT));
        mReleases.add(new SynthParam(mRelease, RELEASE_MIN, RELEASE_MAX, RELEASE_DEFAULT));
        aReleases.add(new SynthParam(aRelease, RELEASE_MIN, RELEASE_MAX, RELEASE_DEFAULT));
    }
    allVecs.push_back(&oscPositions);
    allVecs.push_back(&oscLevels);
    
    allVecs.push_back(&mDelays);
    allVecs.push_back(&aDelays);
    allVecs.push_back(&mAttacks);
    allVecs.push_back(&aAttacks);
    allVecs.push_back(&mHolds);
    allVecs.push_back(&aHolds);
    allVecs.push_back(&mDecays);
    allVecs.push_back(&aDecays);
    allVecs.push_back(&mSustains);
    allVecs.push_back(&aSustains);
    allVecs.push_back(&mReleases);
    allVecs.push_back(&aReleases);
}

void SynthParameterGroup::updateForBlock(apvts &tree)
{
    for(auto vec : allVecs)
    {
        for(auto param : *vec)
        {
            param->setBase(tree);
        }
    }
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
    }
    return layout;
}

