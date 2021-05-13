/*
  ==============================================================================

    SymbolButton.cpp
    Created: 12 May 2021 3:26:03pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "SymbolButton.h"
void PlusButton::setSymbol()
{
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getWidth() / 16;
    auto dY = fBounds.getHeight() / 8;
    symbol.clear();
    symbol.startNewSubPath(5 * dX, 3 * dY);
    symbol.lineTo(7 * dX, 3 * dY);
    symbol.lineTo(7 * dX, 1 * dY);
    symbol.lineTo(9 * dX, 1 * dY);
    symbol.lineTo(9 * dX, 3 * dY);
    symbol.lineTo(11 * dX, 3 * dY);
    symbol.lineTo(11 * dX, 5 * dY);
    symbol.lineTo(9 * dX, 5 * dY);
    symbol.lineTo(9 * dX, 7 * dY);
    symbol.lineTo(7 * dX, 7 * dY);
    symbol.lineTo(7 * dX, 5 * dY);
    symbol.lineTo(5 * dX, 5 * dY);
    symbol.closeSubPath();
}

void MinusButton::setSymbol()
{
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getWidth() / 16;
    auto dY = fBounds.getHeight() / 8;
    symbol.clear();
    symbol.startNewSubPath(5 * dX, 3 * dY);
    symbol.lineTo(11 * dX, 3 * dY);
    symbol.lineTo(11 * dX, 5 * dY);
    symbol.lineTo(5 * dX, 5 * dY);
    symbol.closeSubPath();
}

void NextButton::setSymbol()
{
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getWidth() / 16;
    auto dY = fBounds.getHeight() / 8;
    symbol.clear();
    symbol.startNewSubPath(7 * dX, 2 * dY);
    symbol.lineTo(11 * dX, 4 * dY);
    symbol.lineTo(7 * dX, 6 * dY);
    symbol.closeSubPath();
}

void LastButton::setSymbol()
{
    auto fBounds = getLocalBounds().toFloat();
    auto dX = fBounds.getWidth() / 16;
    auto dY = fBounds.getHeight() / 8;
    symbol.clear();
    symbol.startNewSubPath(11 * dX, 2 * dY);
    symbol.lineTo(11 * dX, 6 * dY);
    symbol.lineTo(7 * dX, 4 * dY);
    symbol.closeSubPath();
}
