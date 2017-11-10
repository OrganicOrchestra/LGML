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


#ifndef INSPECTOREDITOR_H_INCLUDED
#define INSPECTOREDITOR_H_INCLUDED

#include "../../JuceHeaderUI.h"//keep

class InspectableComponent;

class InspectorEditor : public juce::Component, public juce::ComponentListener
{
public:
    InspectorEditor();
    virtual ~InspectorEditor();



    void resized() override;

    virtual int getContentHeight() const;

    virtual void clear();

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

#endif  // INSPECTOREDITOR_H_INCLUDED
