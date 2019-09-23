/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#ifndef SPAT2DVIEWER_H_INCLUDED
#define SPAT2DVIEWER_H_INCLUDED

#include "../../JuceHeaderUI.h"//keep
#include "Spat2DNode.h"

class Spat2DElement;
class CircularHandleComponent : public Component{
public:
    CircularHandleComponent(Spat2DElement * );
    void paint (Graphics& g) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void resized () override;
    bool hitTest (int x, int y) override;
    Label numLabel;
    Spat2DElement * owner;
    float size;
};

class Spat2DElement : public juce::Component
{
public:
    enum Type { SOURCE, TARGET};
    Spat2DElement (Type type, int index, Colour color);
    ~Spat2DElement();

    Type type;
    int index;
    Colour color;

    Point<floatParamType> position;

    void setFloatPosition (const Point<floatParamType> & newPosition);
    void moved () override;

    class  Listener
    {
    public:

        /** Destructor. */
        virtual ~Listener() {}
        virtual void spatElementUserMoved (Spat2DElement* handle, const Point<floatParamType>& newPosition) = 0;
    };

    ListenerList<Listener> elemListeners;
    void addSpatElementListener (Listener* newListener) { elemListeners.add (newListener); }
    void removeSpatElementListener (Listener* listener) { elemListeners.remove (listener); }
    CircularHandleComponent handle;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Spat2DElement)
};

class Spat2DSource : public Spat2DElement
{
public:
    Spat2DSource (int index);
    float angle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Spat2DSource)
};

class Spat2DTarget : public Spat2DElement
{
public:
    Spat2DTarget (int index, Colour color = Colours::orange);
    void setFloatRadius(float r,bool repaint = true);
    float radius;
    float influence;
    void paint (Graphics& g) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Spat2DTarget)
};


class Spat2DViewer : public juce::Component, public Spat2DElement::Listener,
    public ConnectableNode::ConnectableNodeListener,
    public ControllableContainer::Listener,
private Timer
{
public:
    Spat2DViewer (Spat2DNode* node);
    ~Spat2DViewer();

    OwnedArray<Spat2DSource> sources;
    OwnedArray<Spat2DTarget> targets;
    std::unique_ptr<Spat2DTarget> globalTarget;

    Spat2DNode* node;

    void updateNumSources();
    void updateNumTargets();
    void updateTargetRadius();

    void updateSourcePosition (int sourceIndex);
    void updateTargetPosition (int targetIndex);
    void updateTargetInfluence (int targetIndex,bool repaint=true);

    void resized() override;
    void paint (Graphics& g) override;

    void nodeParameterChangedAsync (ConnectableNode*, ParameterBase* p) override;

    void childControllableAdded (ControllableContainer*, Controllable*) override;
    void childControllableRemoved (ControllableContainer*, Controllable*) override;

    // Inherited via Listener (Spat2DElement)
    virtual void spatElementUserMoved (Spat2DElement* handle, const Point<floatParamType>& newPosition) override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Spat2DViewer)

    
    void timerCallback() final; // source, target and intensities



};

#include "Spat2DViewerUI.ipp"
#endif  // SPAT2DVIEWER_H_INCLUDED
