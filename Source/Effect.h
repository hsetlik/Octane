/*
  ==============================================================================

    Effect.h
    Created: 31 May 2021 5:10:47pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#define BITCRUSH_LEVELS 256 //! the finest resolution for the bitcrusher (8 bits = 256 possible values)
enum EffectType
{
    TrueClip,
    BitCrusher,
    ExpOverdrive
};
class Effect
{ //! abstract base class for all effect processors
public:
    Effect(EffectType _type) : type(_type), wetLevel(1.0f) {}
    virtual ~Effect() {}
    virtual float process(float input) {return 0.0f;}
    virtual void setWetDry(float wet) {wetLevel = wet;}
    float processWetDry(float input) //! this goes in the base class bc it should be exactly the same for all effects
    {
        return (process(input) * wetLevel) + (input * (1.0f - wetLevel));
    }
    const EffectType type;
    static std::vector<juce::String> effectStrings;
protected:
    float wetLevel;
};

//================================================================================
class Overdrive : public Effect
{
public:
    Overdrive(EffectType type) : Effect(type), drive(0.0f) {}
    virtual ~Overdrive() {}
    virtual void setDrive(float value) {drive = value;}
protected:
    float drive; //! unit-less parameter between 0 and 1 for UI control. Each \c process() function can convert/use it differently
};

class TrueClip : public Overdrive
{ //! most straightforward clipping. limits inputs to the drive value and then scales -1 - 1
public:
    TrueClip() : Overdrive(EffectType::TrueClip) {}
    float process(float input) override
    {
        output = input;
        if(fabs(input) > drive)
        {
            output = (input > 0.0f) ? drive : drive * -1.0f;
        }
        return output / drive;
    }
private:
    float output;
};

class BitCrusher : public Overdrive
{ //! simple bit crusher. not truly an overdrive but whatever
public:
    BitCrusher() : Overdrive(EffectType::BitCrusher), numLevels(BITCRUSH_LEVELS)
    {
        
    }
    void setDrive(float value) override
    {
        drive = value;
        numLevels = floor(BITCRUSH_LEVELS * (1.0f - drive));
    }
    float process(float input) override
    {
        level = floor(input * numLevels);
        output = (float)level / numLevels;
        return output;
    }
private:
    int level;
    int numLevels;
    float output;
};

class ExpOverdrive : public Overdrive
{
public:
    ExpOverdrive() : Overdrive(EffectType::ExpOverdrive), exponent(1.0f)
    {
    }
    void setDrive(float value) override
    {
        drive = value;
        exponent = 1.0f - drive;
    }
    float exp(float input) {return pow(input, exponent);}
    float process(float input) override
    {
        if(input > 0.0f)
            return exp(input);
        else
        {
            return -exp(-input);
        }
    }
private:
    float output;
    float exponent;
};
//================================================================================
class OctaneEffectProcessor
{
public:
    OctaneEffectProcessor() : lastSample(0.0f) {}
    float process(float input);
    juce::OwnedArray<Effect> allEffects;
private:
    float lastSample;
};
