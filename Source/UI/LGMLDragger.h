/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#if ENGINE_HEADLESS
#error should not include that in headless builds
#endif

#pragma once

#include "../JuceHeaderUI.h"

//DBGinclude
#include "../Controllable/Controllable.h"

class ParameterUI;
class DraggedComponent;

class LGMLDragger : MouseListener,KeyListener
{
public:

    LGMLDragger();
    ~LGMLDragger();
    juce_DeclareSingleton (LGMLDragger, true);

    void setMainComponent (Component* c);

    Component* mainComp;
    

    void registerDragCandidate (ParameterUI* c);
    void unRegisterDragCandidate (ParameterUI* c);


    void mouseEnter (const MouseEvent& e)override;
    void mouseUp (const MouseEvent& e)override;
    void mouseExit (const MouseEvent& e) override;


    void startDraggingComponent (Component* const componentToDrag, const MouseEvent& e);
    void dragComponent (Component* const componentToDrag, const MouseEvent& e, ComponentBoundsConstrainer* const constrainer);
    void endDraggingComponent (Component*   componentToDrag, const MouseEvent& e);



    ScopedPointer<DraggedComponent> dragCandidate;
    void setMappingActive (bool isActive);
    void toggleMappingMode();
    bool isMappingActive;



    Component*  dropCandidate;

    WeakReference<ParameterUI> selected;


    class Listener
    {
    public:
        virtual ~Listener() {};
        virtual void mappingModeChanged(bool) = 0;
        virtual void selectionChanged ( ParameterBase*) = 0;
    };
    void addSelectionListener (Listener* l ) {listeners.add (l);}
    void removeSelectionListener (Listener* l ) {listeners.remove (l);}

    void setSelected (ParameterUI*,LGMLDragger::Listener * from = nullptr);
private:

    Component* selectedSSContent;
    ListenerList<Listener> listeners;
    Point<int> mouseDownWithinTarget;
    bool keyPressed (const KeyPress& key,
                Component* originatingComponent)override;

};
