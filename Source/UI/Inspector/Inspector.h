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


#pragma once

#include "../ShapeShifter/ShapeShifterContent.h"
#include "InspectableComponent.h"
#include "InspectorEditor.h"
#include "../../Controllable/ControllableContainer.h"

class ParameterContainer;
class ParameterBase;

class Inspector : public juce::Component,
    public InspectorEditor::InspectorEditorListener,
    private ControllableContainer::Listener,
private Controllable::Listener,
public SelectedItemSet<WeakReference<InspectableComponent> >

{
public:
    juce_DeclareSingleton (Inspector, true);
    Inspector();
    virtual ~Inspector();



    bool isListening;
    void shouldListen(bool value);


    void clear();




    InspectableComponent * getFirstCurrentComponent();
    ParameterContainer* getFirstCurrentContainerSelected();
    ParameterBase* getFirstCurrentParameterSelected();
    Array<WeakReference<ParameterContainer> >  getContainersSelected();
    bool deselectContainer(ControllableContainer * c);
    void selectComponents(Array<WeakReference<InspectableComponent>> & l);


    void contentSizeChanged (InspectorEditor*) override;

    template<class T>
    Array<WeakReference<T>> getSelectedComponentsOfType();
    template<class T>
    Array<WeakReference<T>> getSelectedContainersOfType();

    //Listener
    class  InspectorListener :private ChangeListener
    {
    public:
        /** Destructor. */
        virtual ~InspectorListener() {}
        virtual void contentSizeChanged (Inspector*) {};
        virtual void selectionChanged(Inspector *){}
    private:
        void changeListenerCallback (ChangeBroadcaster* source) override{
            selectionChanged((Inspector*)source);
        };
        friend class Inspector;

    };

    ListenerList<InspectorListener> listeners;
    void addInspectorListener (InspectorListener* newListener) { addChangeListener(newListener);listeners.add (newListener); }
    void removeInspectorListener (InspectorListener* listener) { removeChangeListener(listener);listeners.remove (listener); }
    int getNumSelected();

private:
    void resized() override;
    void paint(Graphics & g)override;
    void setCurrentComponent (InspectableComponent* component);
    void clearEditor();
    void inspectCurrentComponent();
    void parentHierarchyChanged() override;
    

    std::unique_ptr<InspectorEditor> currentEditor;

    // controllableContainer listener
    void controllableContainerRemoved(ControllableContainer * , ControllableContainer * ) override;
    void containerWillClear(ControllableContainer * )override;

    // controllableListener
    void controllableRemoved (Controllable* ) override;


    void itemSelected (WeakReference<InspectableComponent> c)override;
    void itemDeselected (WeakReference<InspectableComponent> c)override;

    friend class InspectorViewport;

};

template<class T>
Array<WeakReference<T>> Inspector::getSelectedComponentsOfType(){
    Array<WeakReference<T>> res;
    for(auto & c : getItemArray()){
        if(auto * cc = dynamic_cast<T*>(c.get())){
            res.add(WeakReference<T>(cc));
        }
    }
    return res;
}

template<class T>
Array<WeakReference<T>> Inspector::getSelectedContainersOfType(){
    Array<WeakReference<T>> res;
    for(auto & c : getContainersSelected()){
        if(auto * cc = dynamic_cast<T*>(c.get())){
            res.add(WeakReference<T>(cc));
        }
    }
    return res;
}

class InspectorViewport : public ShapeShifterContentComponent, public Inspector::InspectorListener
{
public:
    InspectorViewport (const String& contentName, Inspector* _inspector);

    virtual ~InspectorViewport();

    void resized() override;
    Viewport vp;
    Inspector* inspector;

    void selectionChanged (Inspector*) override { resized(); }
    void contentSizeChanged (Inspector*) override { resized(); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InspectorViewport)
};

