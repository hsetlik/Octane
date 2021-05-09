/*
  ==============================================================================

    LFOEditor.h
    Created: 6 May 2021 5:37:07pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "LFO.h"
#include "ModulationSystem.h"
#include "RgbColor.h"
#define REPAINT_FPS 24
using Constrainer = juce::ComponentBoundsConstrainer;
using fPoint = juce::Point<float>;

class LFOPoint
{
public:
    LFOPoint(float x, float y, bool isBezier, bool isSplit=false) :
    xPos(x),
    yPos(y),
    bezier(isBezier),
    split(isSplit)
    {
        
    }
    void setFrontHandle(float newX, float newY)
    {
        frontHandleX = newX;
        frontHandleY = newY;
        if(!split)
        {
            auto dX = newX - xPos;
            auto dY = newY - yPos;
            rearHandleX = xPos - dX;
            rearHandleY = yPos - dY;
        }
    }
    void setRearHandle(float newX, float newY)
    {
        rearHandleX = newX;
        rearHandleY = newY;
        if(!split)
        {
            auto dX = xPos - newX;
            auto dY = yPos - newY;
            frontHandleX = xPos + dX;
            frontHandleY = yPos + dY;
        }
    }
    void setPosition(float newX, float newY)
    {
        auto dX = newX - xPos;
        auto dY = newY - yPos;
        yPos = newY;
        xPos = newX;
        if(bezier)
        {
            setFrontHandle(xPos + dX, yPos + dX);
            setRearHandle(xPos - dX, yPos - dY);
        }
    }
    fPoint getCenter() {return fPoint(xPos, yPos);}
    fPoint getFront() {return fPoint(frontHandleX, frontHandleY);}
    fPoint getRear() {return fPoint(rearHandleX, rearHandleY);}
    bool isBezier() {return bezier;}
    void setBezier(bool should) {bezier = should; }
    void setSplit(bool should) {split = should; }
private:
    float xPos, yPos;
    bool bezier;
    bool split;
    float frontHandleX;
    float frontHandleY;
    float rearHandleX;
    float rearHandleY;
};

class LFOShape
{
public:
    LFOShape();
    void applyToArray();
    void addPoint(LFOPoint* newPoint)
    {
        points.push_back(newPoint);
    }
    void removePoint(LFOPoint* toRemove)
    {
        int idx = 0;
        for(auto point : points)
        {
            if(point == toRemove)
            {
                points.erase(points.begin() + idx);
                return;
            }
            ++idx;
        }
    }
    std::vector<LFOPoint*> points;
    lfoArray outputArray;
};


class LFOPointHandle : public juce::Component
{
public:
   
};



class LFOEditor : public juce::Component, juce::Timer
{
public:
    LFOEditor(lfoArray* arr);
    void timerCallback() override;
    void paint(juce::Graphics& g) override;
    void resized() override;
    void connectPoints(LFOPoint* first, LFOPoint* second); //!  sets the array's values for this sector of the shape, ensures the corrects paths are drawn
    void addPoint();
    void removePoint(int idx);
    juce::Rectangle<float> fBounds;
    LFOPoint startPoint;
    LFOPoint endPoint;
    juce::OwnedArray<LFOPointHandle> pointHandles;
    juce::OwnedArray<juce::Path> paths;
    lfoArray* const linkedArray;
    lfoArray dataArray;
};
