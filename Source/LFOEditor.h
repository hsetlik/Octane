/*
  ==============================================================================

    LFOEditor.h
    Created: 6 May 2021 5:37:07pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "Synthesizer.h"
#include "ModulationSystem.h"
#include "RgbColor.h"
#include "GraphicsUtility.h"
#include "SymbolButton.h"
#define REPAINT_FPS 24
#define MIN_POINT_DIFFERENCE 0.01f
#define MAX_LFO_HANDLES 10
using Constrainer = juce::ComponentBoundsConstrainer;
using fPoint = juce::Point<float>;

enum class PointType
{
    Linear,
    Bezier,
    Split
};


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
    }
    void setRearHandle(float newX, float newY)
    {
        rearHandleX = newX;
        rearHandleY = newY;
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

class PointHandle : public juce::Component
{
public:
    PointHandle(juce::Component* parent=nullptr, float x=0.5f, float y = 0.5f) :
    dX(0),
    parentComp(parent),
    relativeCenter(fPoint(x, y))
    {
        resized();
        setRepaintsOnMouseActivity(true);
    }
    void resized() override
    {
        setSize(dX, dX);
        constrainer.setMinimumOnscreenAmounts (getHeight(), getWidth(), getHeight(), getWidth());
    }
    void paint(juce::Graphics& g) override
    {
        g.setColour(UXPalette::lightGray);
        g.fillEllipse(getLocalBounds().toFloat());
    }
    void mouseDown(const juce::MouseEvent& e) override
    {
        getParentComponent()->mouseDown(e);
        dragger.startDraggingComponent(this, e);
        
    }
    void mouseUp(const juce::MouseEvent& e) override
    {
        getParentComponent()->mouseDown(e);
    }
    void mouseDrag(const juce::MouseEvent& e) override
    {
        dragger.dragComponent(this, e, &constrainer);
    }
    void moved() override
    {
        if(parentComp == nullptr)
            relativeCenter = getBounds().getCentre().toFloat() / juce::Point<int>(getParentWidth(), getParentHeight()).toFloat();
        else
        {
            relativeCenter = getBounds().getCentre().toFloat() / juce::Point<int>(parentComp->getWidth(), parentComp->getHeight()).toFloat();
        }
    }
    fPoint getCenter()
    {
        return relativeCenter;
    }
    float relativeX() {return relativeCenter.x; }
    fPoint getCenter(juce::Component* container)
    {
        return fPoint(container->proportionOfWidth(relativeCenter.x),
                      container->proportionOfHeight(relativeCenter.y));
    }
    void setCenter(float x, float y)
    {
        if(x < 0.05f)
            x = 0.05f;
        if(x > 0.98f)
            x = 0.98f;
        if(y < 0.05f)
            y = 0.05f;
        if(y > 0.98f)
            y = 0.98f;
        relativeCenter = fPoint(x, y);
        if(parentComp != nullptr)
        {
            auto aX = parentComp->proportionOfWidth(relativeCenter.x);
            auto aY = parentComp->proportionOfWidth(relativeCenter.y);
            setCentrePosition(aX, aY);
        }
    }
    void setCenter(float x, float y, juce::Component* container)
    {
        setCenter(x, y);
        auto aX = container->proportionOfWidth(relativeCenter.x);
        auto aY = container->proportionOfWidth(relativeCenter.y);
        setCentrePosition(aX, aY);
    }
    void startDrag(const juce::MouseEvent& e) {dragger.startDraggingComponent(this, e); }
    void drag(const juce::MouseEvent& e) {dragger.dragComponent(this, e, &constrainer); }
    int dX;
    Constrainer constrainer;
    juce::ComponentDragger dragger;
    juce::Component* parentComp; //! so that the center point can be relative to a higher level component rather than the immediate parent
protected:
    fPoint relativeCenter;
};

class CurveHandle : public PointHandle
{
public:
    CurveHandle(juce::Component* container=nullptr) : PointHandle(nullptr)
    {
        
    }
    void paint(juce::Graphics& g) override
    {
        g.setColour(UXPalette::lightGray);
        g.fillEllipse(getLocalBounds().toFloat());
    }
};

class CenterHandle : public PointHandle
{
public:
    CenterHandle(juce::Component* container=nullptr, PointType type=PointType::Linear, float x=0.5f, float y=0.5f) :
    PointHandle(container, x, y),
    parentComp(container),
    currentType(type),
    needsHandles((type != PointType::Linear)),
    needsHandlesRemoved(false),
    handles(nullptr)
    {
        
    }
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override
    {
        getParentComponent()->mouseDown(e);
        if(e.mods.isRightButtonDown())
        {
            nextHandleType();
            return;
        }
        dragger.startDraggingComponent(this, e);
        if(handles != nullptr)
        {
            handles->front->startDrag(e);
            handles->rear->startDrag(e);
        }
    }
    
    void mouseDrag(const juce::MouseEvent& e) override
    {
        dragger.dragComponent(this, e, &constrainer);
        if(handles != nullptr)
        {
            handles->front->drag(e);
            handles->rear->drag(e);
        }
    }
    fPoint getLeadHandlePoint(juce::Component* container)
    {
        if(isBezier())
        {
            auto pCenter = handles->front->getCenter();
            return fPoint((float)container->proportionOfWidth(pCenter.x), (float)container->proportionOfHeight(pCenter.y));
        }
        return fPoint(0.0f, 0.0f);
    }
    fPoint getRearHandlePoint(juce::Component* container)
    {
        if(isBezier())
        {
            auto pCenter = handles->rear->getCenter();
            return fPoint((float)container->proportionOfWidth(pCenter.x), (float)container->proportionOfHeight(pCenter.y));
        }
        return fPoint(0.0f, 0.0f);
    }
    bool isBezier() {return (currentType != PointType::Linear); }
    juce::Path outline;
    juce::Component* const parentComp;
    PointType currentType;
    bool needsHandles;
    bool needsHandlesRemoved;
    //=====================================================================================================
    class CurveHandlePair : public juce::ComponentListener
    {
    public:
        CurveHandlePair(CenterHandle* center, juce::Component* parent, bool split=false) :
        linkedCenter(center),
        parentComp(parent),
        front(std::make_unique<CurveHandle>(parent)),
        rear(std::make_unique<CurveHandle>(parent)),
        isSplit(split),
        xDelta(0.2f),
        yDelta(0.0f)
        {
            front->addComponentListener(this);
            rear->addComponentListener(this);
        }
        ~CurveHandlePair()
        {
            front->getParentComponent()->removeChildComponent(&*front);
            rear->getParentComponent()->removeChildComponent(&*rear);
        }
        void makeVisible()
        {
            parentComp->addAndMakeVisible(*front);
            parentComp->addAndMakeVisible(*rear);
        }
        void makeInvisible()
        {
            front->setVisible(false);
            front->setEnabled(false);
            rear->setVisible(false);
            rear->setEnabled(false);
        }
        void componentMovedOrResized(juce::Component& comp, bool wasMoved, bool wasResized) override;
        
        void setDeltas()
        {
            auto pCenter = linkedCenter->getCenter();
            xCenter = pCenter.x;
            yCenter = pCenter.y;
            auto lCenter = front->getCenter();
            xDelta = lCenter.x - pCenter.x;
            yDelta = lCenter.y - pCenter.y;
        }
        fPoint idealFrontPos();
        fPoint idealRearPos();
        CenterHandle* const linkedCenter;
        juce::Component* const parentComp;
        std::unique_ptr<CurveHandle> front;
        std::unique_ptr<CurveHandle> rear;
    private:
        bool isSplit;
        float xCenter, yCenter;
        float xDelta, yDelta; //! to represent the distance between the front handle and the center
    };
    //=======================================================================================
    CurveHandlePair* makeHandlePair();
    CurveHandlePair* handles;
    void nextHandleType();
    
};
//==========================================================================================
class LFOEditor : public juce::Component, juce::Timer
{
public:
    LFOEditor(lfoArray* arr, OctaneUpdater* updater, int idx);
    ~LFOEditor();
    void timerCallback() override;
    void paint(juce::Graphics& g) override;
    void resized() override;
    void connectPoints(CenterHandle* first, CenterHandle* second); //!  sets the array's values for this sector of the shape, ensures the corrects paths are drawn
    void setArray();
    void addPoint(); //! TODO: write something to find the largest gap between points and put a point in the middle
    void removePoint();
    void calculatePaths();
    void updateHandles();
    void setHandlesFor(CenterHandle* center);
    void removePoint(int idx);
    void mouseDown(const juce::MouseEvent& e) override;
    juce::Rectangle<float> fBounds;
    LFOPoint startPoint;
    LFOPoint endPoint;
    juce::OwnedArray<CenterHandle> centerHandles;
    juce::OwnedArray<CenterHandle::CurveHandlePair> handlePairs;
    juce::OwnedArray<juce::Path> paths;
    lfoArray* const linkedArray;
    lfoArray dataArray;
    void removeHandlesFrom(CenterHandle* center)
    {
        for(auto pair : handlePairs)
        {
            if(pair->linkedCenter == center)
            {
                center->handles = nullptr;
                handlePairs.removeObject(pair);
                return;
            }
        }
    }
    struct PointGap //! to keep track of gaps between the points so new points can be added in the correct place
    {
        int lowerIdx;
        int upperIdx;
        float gapSize;
        PointGap(int lower, int upper, float size) :
        lowerIdx(lower),
        upperIdx(upper),
        gapSize(size)
        {
        }
    };
    static PointGap largestGap(std::vector<PointGap>& vec)
    {
        int largestIdx = 0;
        float largestGap = std::numeric_limits<float>::min();
        int idx = 0;
        for(auto gap : vec)
        {
            if(gap.gapSize > largestGap)
            {
                largestGap = gap.gapSize;
                largestIdx = idx;
            }
            ++idx;
        }
        return vec[largestIdx];
    }
private:
    int frameIndex;
    OctaneUpdater* const linkedUpdater;
    const int lfoIndex;
};
//==========================================================================
class LFOEditorPanel :
public juce::Component,
public juce::Button::Listener
{
public:
    LFOEditorPanel(lfoArray* array, OctaneUpdater* updater, int index);
    void resized() override;
    void paint(juce::Graphics& g) override;
    void buttonClicked(juce::Button* b) override;
private:
    OctaneUpdater* const linkedUpdater;
    PlusButton addButton;
    MinusButton delButton;
    std::unique_ptr<LFOEditor> editor;
    const int lfoIndex;
};
