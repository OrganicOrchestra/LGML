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


#ifndef SHAPESHIFTERPANELHEADER_H_INCLUDED
#define SHAPESHIFTERPANELHEADER_H_INCLUDED

#include "ShapeShifterPanelTab.h"
#include "../../UI/Style.h"

class ShapeShifterPanelHeader :
    public juce::Component,
    public ShapeShifterPanelTab::TabListener,
    public ButtonListener
{
public:
    ShapeShifterPanelHeader();
    virtual ~ShapeShifterPanelHeader();

    OwnedArray<ShapeShifterPanelTab> tabs;
    AddElementButton addPannelMenu;
    void buttonClicked (Button*)override;

    void addTab (ShapeShifterContent* content);
    void removeTab (ShapeShifterPanelTab* tab, bool doRemove = true);
    void attachTab (ShapeShifterPanelTab* tab);

    ShapeShifterPanelTab* getTabForContent (ShapeShifterContent* content);


    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;

    void paint (Graphics& g) override;
    void resized()override;

    void askForRemoveTab (ShapeShifterPanelTab*) override;

    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void tabDrag (ShapeShifterPanelTab*) = 0;
        virtual void tabSelect (ShapeShifterPanelTab*) = 0;

        virtual void askForRemoveTab (ShapeShifterPanelTab*) = 0;
        virtual void headerDrag() = 0;


    };

    ListenerList<Listener> listeners;
    void addHeaderListener (Listener* newListener) { listeners.add (newListener); }
    void removeHeaderListener (Listener* listener) { listeners.remove (listener); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShapeShifterPanelHeader)
};


#endif  // SHAPESHIFTERPANELHEADER_H_INCLUDED
