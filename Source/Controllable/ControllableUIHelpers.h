/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#pragma once

#include "../JuceHeaderCore.h"
#if !ENGINE_HEADLESS
#include "../JuceHeaderUI.h"//keep



class ControllableContainer;
class Controllable;


class ControllableContainerPopupMenu : public PopupMenu
{
public:
    ControllableContainerPopupMenu (ControllableContainer* rootContainer, Array<Controllable*>* filterOut = nullptr,int startId=1);
    virtual ~ControllableContainerPopupMenu();

    Array<Controllable*> controllableList;
    Array<Controllable*>* filterOutControllable;
    int startId,endId;
    void populateMenu (PopupMenu* subMenu, ControllableContainer* container, int& currentId);


    Controllable* showAndGetControllable();
    Controllable* getControllableForResult(int result);

};


class ControllableReferenceUI : public TextButton, public Button::Listener
{
public :
    ControllableReferenceUI (ControllableContainer* rootContainer = nullptr, Array<Controllable*> filterOut = {});
    virtual ~ControllableReferenceUI();

    WeakReference<ControllableContainer> rootContainer;

    WeakReference<Controllable> currentControllable;
    void setCurrentControllable (Controllable* c);

    Array<Controllable*> filterOutControllable;



    class  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}
        virtual void choosedControllableChanged (ControllableReferenceUI*, Controllable*) {}
    };

    ListenerList<Listener> listeners;
    void addControllableReferenceUIListener (Listener* newListener) { listeners.add (newListener); }
    void removeControllableReferenceUIListener (Listener* listener) { listeners.remove (listener); }

    virtual void buttonClicked (Button* b) override;
};


template<class FunctionType>
bool execOrDefer (FunctionType func)
{
    if (MessageManager::getInstance()->isThisTheMessageThread())
    {
        func();
        return true;
    }
    else
    {
        MessageManager::callAsync (func);
        return false;
    }
}


#endif  // ENGINE_HEADLESS
