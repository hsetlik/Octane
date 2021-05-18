/*
  ==============================================================================

    SymbolButton.h
    Created: 12 May 2021 3:26:03pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "RgbColor.h"
#include "CustomLnFs.h"

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

class NextButton : public SymbolButton
{
public:
    NextButton() : SymbolButton("next_button")
    {
        setRounded(true, 8);
    }
    void setSymbol() override;
};

class LastButton : public SymbolButton
{
public:
    LastButton() : SymbolButton("last_button")
    {
        setRounded(true, 8);
    }
    void setSymbol() override;
};

class PowerButtonCore : public juce::ShapeButton
{
public:
    PowerButtonCore(juce::String name = "power_button", juce::Colour bkgnd = UXPalette::darkGray) : juce::ShapeButton(name, bkgnd, bkgnd, bkgnd)
    {
        setClickingTogglesState(true);
        triggerClick();
    }
    void resized() override
    {
        auto fBounds = getLocalBounds().toFloat();
        auto cX = fBounds.getWidth() / 2.0f;
        auto cY = fBounds.getHeight() / 2.0f;
        auto rX = cX * 0.8f;
        auto rY = cY * 0.8f;
        auto dAngle = juce::MathConstants<float>::pi / 4.0f;
        circle.clear();
        circle.addCentredArc(cX, cY, rX, rY, 0.0f, dAngle, juce::MathConstants<float>::twoPi - dAngle, true);
        line.clear();
        line.startNewSubPath(cX, cY * 0.2f);
        line.lineTo(cX, cY * 1.25f);
    }
    void paintButton(juce::Graphics& g, bool highlighted, bool down) override
    {
        g.setColour(UXPalette::lightGray);
        g.fillEllipse(getLocalBounds().toFloat());
        auto stroke = juce::PathStrokeType(2.0f);
        g.setColour(UXPalette::darkGray);
        if(getToggleState())
            g.setColour(UXPalette::highlight);
        g.strokePath(line, stroke);
        g.strokePath(circle, stroke);
    }
private:
    juce::Path line;
    juce::Path circle;
};



namespace OctaneButtons
{
    class Text : public juce::TextButton
    {
    public:
        Text(juce::String text=" ")
        {
            setButtonText(text);
            setLookAndFeel(&lnf);
        }
        ~Text()
        {
            setLookAndFeel(nullptr);
        }
        void resized() override
        {
            setBounds(getBounds());
        }
    private:
        DropdownLookAndFeel lnf;
    };
};
