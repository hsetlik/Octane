/*
  ==============================================================================

    DAHDSR.h
    Created: 6 Oct 2020 1:07:11pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

// macros to define parameter limits
// these should be here in every header that defines an object which is associated with parameters
#define DELAY_MIN 0.0f
#define DELAY_MAX 20000.0f
#define DELAY_DEFAULT 0.0f
#define DELAY_CENTER 1000.0f

#define ATTACK_MIN 0.0f
#define ATTACK_MAX 20000.0f
#define ATTACK_DEFAULT 20.0f
#define ATTACK_CENTER 1000.0f

#define HOLD_MIN 0.0f
#define HOLD_MAX 20000.0f
#define HOLD_DEFAULT 0.0f
#define HOLD_CENTER 1000.0f

#define DECAY_MIN 0.0f
#define DECAY_MAX 20000.0f
#define DECAY_DEFAULT 40.0f
#define DECAY_CENTER 1000.0f

#define SUSTAIN_MIN 0.0f
#define SUSTAIN_MAX 1.0f
#define SUSTAIN_DEFAULT 0.6f

#define RELEASE_MIN 0.0f
#define RELEASE_MAX 20000.0f
#define RELEASE_DEFAULT 80.0f
#define RELEASE_CENTER 1000.0f

class DAHDSR
{
public:
    enum envPhase
    {
        delayPhase,
        attackPhase,
        holdPhase,
        decayPhase,
        sustainPhase,
        releasePhase,
        noteOff
    };
    //functions
    DAHDSR(int ind=0) : factor(1.0f), sampleRate(44100), index(ind)
    {
        trigger = false;
        samplesIntoPhase = 0;
        currentPhase = noteOff;
    }
    ~DAHDSR() {}
    static envPhase nextPhase(envPhase input)
    {
        if(input != noteOff)
            return (envPhase)(input + 1);
        else
            return noteOff;
    }
    void triggerOn()
    {
        trigger = true;
        enterPhase(delayPhase);
    }
    float factorFor(float startLevel, float endLevel, float lengthMs)
    {
        if(startLevel == 0.0f)
            startLevel = minLevel;
        if(endLevel == 0.0f)
            endLevel = minLevel;
        unsigned long phaseLengthSamples = lengthMs * (sampleRate / 1000);
        return exp((log(endLevel) - log(startLevel)) / phaseLengthSamples);
    }
    void triggerOff()
    {
        trigger = false;
        enterPhase(releasePhase);
    }
    void updatePhase()
    {
        if(samplesIntoPhase > samplesInPhase || samplesInPhase < 1)
        {
            enterPhase(nextPhase(currentPhase));
        }
    }
    void enterPhase(envPhase newPhase);
    void setSampleRate(double value) {sampleRate = value;}
    float process(float input);
    float clockOutput()
    {
        return process(1.0f);
    }
    envPhase getPhase() {return currentPhase;}
    double output;
    void setDelay(float val) {delayTime = val;}
    void setAttack(float val) {attackTime = val;}
    void setHold(float val) {holdTime = val;}
    void setDecay(float val) {decayTime = val;}
    void setSustain(float val) {sustainLevel = val;}
    void setRelease(float val) {releaseTime = val;}
private:
    //data
    envPhase currentPhase;
    unsigned long long samplesIntoPhase;
    unsigned long long samplesInPhase;
    double factor;
    float minLevel = 0.00001f;
    double sampleRate;
    int index;
    bool trigger;
    float delayTime = DELAY_DEFAULT;
    float attackTime = ATTACK_DEFAULT;
    float holdTime = HOLD_DEFAULT;
    float decayTime = DECAY_DEFAULT;
    float sustainLevel = SUSTAIN_DEFAULT;
    float releaseTime = RELEASE_DEFAULT;
    float _startLevel;
    float _endLevel;
};
