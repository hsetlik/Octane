/*
  ==============================================================================

    RgbColor.h
    Created: 6 Feb 2021 4:10:08pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
struct ColorTriad
{
    juce::Colour left, right, center;
};

struct ColorData {
    juce::Colour color;
    juce::String desc;
};

struct ColorSet {
    std::vector<ColorData> colors;
    void add(juce::Colour c, juce::String d)
    {
        ColorData data;
        data.color = c;
        data.desc = d;
        if(colors.size() > 0)
        {
            int numNameMatches = 0;
            for(auto i : colors) //appends a number if the name is the same as any already in the set
            {
                if(i.desc.contains(data.desc))
                    ++numNameMatches;
            }
            if(numNameMatches > 0)
                data.desc += juce::String(numNameMatches);
        }
        colors.push_back(data);
    }
    void add(ColorData& d)
    {
        ColorData newCol = d;
        if(colors.size() > 0)
        {
            int numNameMatches = 0;
            for(auto i : colors)
            {
                if(i.desc.contains(newCol.desc))
                    ++numNameMatches;
            }
            if(numNameMatches > 0)
                newCol.desc += juce::String(numNameMatches);
        }
        colors.push_back(newCol);
    }
    void add(ColorSet s)
    {
        for(auto i : s.colors)
        {
            add(i);
        }
    }
    juce::Colour getByDesc(juce::String d)
    {
        for(auto i : colors)
        {
            if(i.desc == d)
                return i.color;
        }
        return juce::Colours::black;
    }
    
    juce::Colour getByDesc(const char* s)
    {
        for(auto i : colors)
        {
            if(i.desc.toRawUTF8() == s)
                return i.color;
        }
        return juce::Colours::black;
    }
    
    juce::Colour getByDesc(std::string s)
    {
        for(auto i : colors)
        {
            if(i.desc.toStdString() == s)
                return i.color;
        }
        return juce::Colours::black;
    }
    juce::Colour atIndex(int index)
    {
        return colors[index].color;
    }
    void set(int index, juce::Colour col)
    {
        colors[index].color = col;
    }
    void clear()
    {
        colors.clear();
    }
};


struct Color
{
    //fourth argument sets alpha, fully opaque by default
    static juce::Colour RGBColor(int r, int g, int b, int a = 255)
    {
        return(juce::Colour(juce::uint8(r), juce::uint8(g), juce::uint8(b), juce::uint8(a)));
    }
    
    static juce::Colour blendRGB(juce::Colour colorA, juce::Colour colorB, float ratio)
    {
        auto fRed = lerp(colorA.getFloatRed(), colorB.getFloatRed(), ratio);
        auto fGreen = lerp(colorA.getFloatGreen(), colorB.getFloatGreen(), ratio);
        auto fBlue = lerp(colorA.getFloatBlue(), colorB.getFloatBlue(), ratio);
        auto fAlpha = lerp(colorA.getFloatAlpha(), colorB.getFloatAlpha(), ratio);
        return juce::Colour(fRed, fGreen, fBlue, fAlpha);
    }
    static juce::Colour blendHSB(juce::Colour a, juce::Colour b, float ratio)
    {
        auto fHue = lerp(a.getHue(), b.getHue(), ratio);
        auto fSat = lerp(a.getSaturation(), b.getSaturation(), ratio);
        auto fBrt = lerp(a.getBrightness(), b.getBrightness(), ratio);
        auto fAlpha = lerp(a.getFloatAlpha(), b.getFloatAlpha(), ratio);
        return juce::Colour(fHue, fSat, fBrt, fAlpha);
    }
    
    static juce::Colour complement(juce::Colour start)
    {
        auto fHue = start.getHue();
        auto fSat = start.getSaturationHSL();
        auto fLgt = start.getLightness();
        auto dHueStart = fabs(1.0f - fHue);
        return juce::Colour(dHueStart, fSat, fLgt, 1.0f);
    }
    static juce::Colour desaturated(juce::Colour input, float amt)
    {
        return juce::Colour(input.getHue(), input.getSaturation() * amt, input.getBrightness(), input.getFloatAlpha());
    }
    static ColorSet triadFrom(juce::Colour center)
    {
        ColorSet set;
        ColorTriad triad;
        triad.center = center;
        set.add(triad.center, "Center");
        auto fHue = center.getHue() * 360;
        auto rHue = fHue + 120;
        if(rHue > 360)
            rHue -= 360;
        else if(rHue < 0)
            rHue += 360;
        triad.right = juce::Colour(rHue, center.getSaturationHSL(), center.getLightness(), 1.0f);
        set.add(triad.right, "Right");
        auto lHue = fHue - 120;
        if(lHue > 360)
            lHue -= 360;
        else if(lHue < 0)
            lHue += 360;
        triad.left = juce::Colour(lHue, center.getSaturationHSL(), center.getLightness(), 1.0f);
        set.add(triad.left, "Left");
        return set;
    }
    
    static ColorSet monochromeFrom(juce::Colour center, bool skewSaturation=false, juce::String prefix="")
    {
        auto fHue = center.getHue();
        auto fSat = center.getSaturation();
        auto fLgt = center.getLightness();
        auto increment = (fLgt - 0.1f) / 5.0f; //note: center color must have a lightness value grater than 0.1 to start
        ColorSet set;
        for(int i = 0; i < 5; ++i)
        {
            if(skewSaturation)
                fSat = 0.15f + (0.12 * i);
           set.add(juce::Colour(fHue, fSat, (fLgt - (i * increment)), 1.0f), prefix + "ColorL" + juce::String(i));
        }
        return set;
    }
    static ColorSet shadesBetween(juce::Colour a, juce::Colour b, int numShades)
    {
        ColorSet set;
        set.add(a, "gradientStart");
        auto startR = a.getFloatRed();
        auto dR = (b.getFloatRed() - a.getFloatRed()) / numShades;
        auto startG = a.getFloatGreen();
        auto dG = (b.getFloatGreen() - a.getFloatGreen()) / numShades;
        auto startB = a.getFloatBlue();
        auto dB = (b.getFloatBlue() - a.getFloatBlue());
        for(int i = 1; i < (numShades - 1); ++i)
        {
            int iR = startR + (dR * i);
            int iG = startG + (dG * i);
            int iB = startB + (dB * i);
            auto name = "gradientShade" + juce::String(i);
            set.add(RGBColor(iR, iG, iB), name);
        }
        set.add(b, "gradientEnd");
        return set;
    }
    static std::vector<juce::Colour> shadesBetweenVec(juce::Colour a, juce::Colour b, int numShades)
    {
        
        std::vector<juce::Colour> output;
        auto inc = 1.0f / (float) numShades;
        for(int i = 1; i < (numShades - 1); ++i)
        {
            output.push_back(blendHSB(a, b, inc * i));
        }
        return output;
    }
private:
    static int lerp(int a, int b, float ratio)
    {
        return (a * (1.0f - ratio)) + (b * ratio);
    }
    static float lerp(float a, float b, float ratio)
    {
        return (a * (1.0f - ratio)) + (b * ratio);
    }
};
