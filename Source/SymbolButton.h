/*
  ==============================================================================

    SymbolButton.h
    Created: 12 May 2021 3:26:03pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "RgbColor.h"

class SymbolButton : public juce::ShapeButton
{
public:
    SymbolButton(juce::String name="symbol_button", juce::Colour bkgnd= UXPalette::lightGray, juce::Colour sColour= UXPalette::highlight) :
    juce::ShapeButton(name, bkgnd, bkgnd, bkgnd),
    upBkgnd(bkgnd),
    downBkgnd(bkgnd.darker(0.3f)),
    symbolColor(sColour),
    isRounded(false),
    cornerSize(1.0f),
    cornerFactor(6)
    {
        resized();
    }
    virtual ~SymbolButton() {}
    void paintButton(juce::Graphics& g, bool highlighted, bool down) override
    {
        if(down)
            g.setColour(downBkgnd);
        else
            g.setColour(upBkgnd);
        g.fillPath(bkgnd);
        g.setColour(symbolColor);
        g.fillPath(symbol);
    }
    void resized() override
    {
        auto fBounds = getLocalBounds().toFloat();
        cornerSize = fBounds.getWidth() / (float)cornerFactor;
        bkgnd.clear();
        if(isRounded)
            bkgnd.addRoundedRectangle(fBounds, cornerSize);
        else
            bkgnd.addRectangle(fBounds);
        symbol.clear();
        setSymbol();
    }
    void setRounded(bool shouldBeRounded, int corner=6)
    {
        isRounded = shouldBeRounded;
        cornerFactor = corner;
        resized();
    }
    virtual void setSymbol() {}
protected:
    juce::Path symbol;
private:
    juce::Path bkgnd;
    juce::Colour upBkgnd;
    juce::Colour downBkgnd;
    juce::Colour symbolColor;
    bool isRounded;
    float cornerSize;
    int cornerFactor;
};

class PlusButton : public SymbolButton
{
public:
    PlusButton() : SymbolButton("plus_button")
    {
        setRounded(true, 8);
    }
    void setSymbol() override;
};

class MinusButton : public SymbolButton
{
public:
    MinusButton() : SymbolButton("minus_button")
    {
        setRounded(true, 8);
    }
    void setSymbol() override;
};
