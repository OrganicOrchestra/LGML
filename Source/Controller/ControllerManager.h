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

#ifndef CONTROLMANAGER_H_INCLUDED
#define CONTROLMANAGER_H_INCLUDED


#include "ControllerFactory.h"
#include "../Controllable/Parameter/ParameterContainer.h"
#include "Controller.h"

class ControllerManager :  public ParameterContainer
{
public:

    ControllerManager();
    ~ControllerManager();

    juce_DeclareSingleton (ControllerManager, true);

    ControllerFactory factory;
    OwnedArray<Controller> controllers;

    Controller* addController (Controller* controller);
    void removeController (Controller* c);
    void clear();


    ParameterContainer*   addContainerFromObject (const String& name, DynamicObject*  )override;

    class  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}

        virtual void controllerAdded (Controller*) = 0;
        virtual void controllerRemoved (Controller*) = 0;
    };

    ListenerList<Listener> listeners;
    void addControllerListener (Listener* newListener) { listeners.add (newListener); }
    void removeControllerListener (Listener* listener) { listeners.remove (listener); }





    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControllerManager)
};






#endif  // CONTROLMANAGER_H_INCLUDED
