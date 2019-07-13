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

//#include "NodeConnectionEditorDataSlot.h"//keep
//==============================================================================
/*
*/
class NodeConnectionEditorLink    : public juce::Component,
    public SettableTooltipClient
{
public:
    NodeConnectionEditorLink (NodeConnectionEditorDataSlot* outSlot, NodeConnectionEditorDataSlot* inSlot);
    ~NodeConnectionEditorLink();

    NodeConnectionEditorDataSlot* outSlot;
    NodeConnectionEditorDataSlot* inSlot;

    bool isSelected;
    void setSelected (bool value)
    {
        isSelected = value;
        DBG ("repaint");
        repaint();
    }

    bool isEditing;
    void setEditing (bool value)
    {
        isEditing = value;
        repaint();
    }

    NodeConnectionEditorDataSlot* getBaseSlot()
    {
        return outSlot == nullptr ? inSlot : outSlot;
    }

    NodeConnectionEditorDataSlot* candidateDropSlot;

    bool finishEditing();
    bool setCandidateDropSlot (NodeConnectionEditorDataSlot* connector);
    void cancelCandidateDropSlot();

    virtual bool hitTest (int x, int y) override { return hitPath.contains ((float)x, (float)y); }

    Path hitPath;
    void paint (Graphics&)override;
    void resized()override;

    void mouseEnter (const MouseEvent&) override;
    void mouseExit (const MouseEvent&) override;
    void mouseDown (const MouseEvent&) override;

    void mouseDoubleClick (const MouseEvent&) override;
    bool keyPressed (const KeyPress& key) override;

    void remove();

    //Listener
    class LinkListener
    {
    public:
        /** Destructor. */
        virtual ~LinkListener() {}
        virtual void askForRemoveLink (NodeConnectionEditorLink* target) = 0;
        virtual void selectLink (NodeConnectionEditorLink* link) = 0;
    };

    ListenerList<LinkListener> listeners;
    void addLinkListener (LinkListener* newListener) { listeners.add (newListener); }
    void removeLinkListener (LinkListener* listener) { listeners.remove (listener); }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeConnectionEditorLink)
};

#include "NodeConnectionEditorLink.ipp"
