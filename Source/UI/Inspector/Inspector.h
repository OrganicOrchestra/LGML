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


#ifndef INSPECTOR_H_INCLUDED
#define INSPECTOR_H_INCLUDED

#include "../ShapeShifter/ShapeShifterContent.h"
#include "InspectableComponent.h"
#include "InspectorEditor.h"
#include "../../Controllable/Parameter/ParameterContainer.h"

class Inspector : public juce::Component,
    public InspectorEditor::InspectorEditorListener,
    private ControllableContainer::Listener

{
public:
    juce_DeclareSingleton (Inspector, false);
    Inspector();
    virtual ~Inspector();



    bool isEnabled;
    void setEnabled (bool value);


    void clear();

    void setCurrentComponent (InspectableComponent* component);

    InspectableComponent * getCurrentComponent();
    ParameterContainer* getCurrentContainerSelected();

    InspectorEditor * getCurrentEditor();


    void resized() override;

    void clearEditor();
    void inspectCurrentComponent();

    

    void contentSizeChanged (InspectorEditor*) override;
    //Listener
    class  InspectorListener
    {
    public:
        /** Destructor. */
        virtual ~InspectorListener() {}
        virtual void currentComponentChanged (Inspector* ) {};
        virtual void contentSizeChanged (Inspector*) {};
    };

    ListenerList<InspectorListener> listeners;
    void addInspectorListener (InspectorListener* newListener) { listeners.add (newListener); }
    void removeInspectorListener (InspectorListener* listener) { listeners.remove (listener); }

private:
    WeakReference<InspectableComponent> currentComponent;

    ScopedPointer<InspectorEditor> currentEditor;

    SelectedItemSet<WeakReference<InspectableComponent>> selectedComps;
    
    void controllableContainerRemoved(ControllableContainer * , ControllableContainer * ) override;
    void containerWillClear(ControllableContainer * )override;
};

class InspectorViewport : public ShapeShifterContentComponent, public Inspector::InspectorListener
{
public:
    InspectorViewport (const String& contentName, Inspector* _inspector) : inspector (_inspector), ShapeShifterContentComponent (contentName)
    {
        vp.setViewedComponent (inspector, false);
        vp.setScrollBarsShown (true, false);
        vp.setScrollOnDragEnabled (false);
        contentIsFlexible = false;
        addAndMakeVisible (vp);
        vp.setScrollBarThickness (10);

        inspector->addInspectorListener (this);

    }

    virtual ~InspectorViewport()
    {
        Inspector::deleteInstance();
    }

    void resized() override
    {
        
        Rectangle<int> r = getLocalBounds();

        vp.setBounds (r);

        r.removeFromRight (vp.getScrollBarThickness());

        if (inspector->getCurrentEditor() == nullptr) inspector->setBounds (r);
        else
        {
            int cH = inspector->getCurrentEditor()->getContentHeight();

            if (cH == 0) cH = r.getHeight();

            inspector->setBounds (r.withPosition (inspector->getPosition()).withHeight (cH));
        }
    }
    Viewport vp;
    Inspector* inspector;

    void currentComponentChanged (Inspector*) override { resized(); }
    void contentSizeChanged (Inspector*) override { resized(); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InspectorViewport)
};

#endif  // INSPECTOR_H_INCLUDED
