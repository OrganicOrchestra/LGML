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

#include "../../JuceHeaderUI.h"//keep



class InspectorEditor : public juce::Component, public juce::ComponentListener
{
public:
    InspectorEditor(){setOpaque(true);};
    virtual ~InspectorEditor(){};

    void paint(Graphics & g)override;

    virtual int getContentHeight() const = 0;

    virtual void clear(){};

    class  InspectorEditorListener
    {
    public:
        /** Destructor. */
        virtual ~InspectorEditorListener() {}
        virtual void contentSizeChanged (InspectorEditor*) {};
    };

    ListenerList<InspectorEditorListener> inspectorEditorListeners;
    void addInspectorEditorListener (InspectorEditorListener* newListener) { inspectorEditorListeners.add (newListener); }
    void removeInspectorEditorListener (InspectorEditorListener* listener) { inspectorEditorListeners.remove (listener); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InspectorEditor)
};

class ComponentInspectorEditor : public InspectorEditor{
public:
    ComponentInspectorEditor(Component * c,int _height):component(c),height(_height){
        addAndMakeVisible(c);
    }
private:
    void resized() override{component->setBounds(getLocalBounds());}
    int getContentHeight() const override{return height;}

    int height;
    std::unique_ptr<Component> component;
};

