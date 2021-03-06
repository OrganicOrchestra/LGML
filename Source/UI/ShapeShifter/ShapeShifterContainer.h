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

#include "ShapeShifterPanel.h"
#include "GapGrabber.h"


class ShapeShifterContainer;

class ShapeShifterContainerListener
{
public:
    virtual ~ShapeShifterContainerListener() {}
    virtual void panelAdded (ShapeShifterContainer*) {}
    virtual void panelRemoved (ShapeShifterContainer*) {}

    virtual void containerAdded (ShapeShifterContainer*) {}
    virtual void containerRemoved (ShapeShifterContainer*) {}

    virtual void containerEmptied (ShapeShifterContainer*) {}
    virtual void oneShifterRemaining (ShapeShifterContainer*, ShapeShifter*) {}
};


//Listener
class ShapeShifterContainer : public ShapeShifter,
    public GapGrabber::Listener,
    public ShapeShifterPanel::Listener,
    public ShapeShifterContainerListener
{
public:
    enum Direction { NONE, HORIZONTAL, VERTICAL};
    enum Position { TOP, BOTTOM, LEFT, RIGHT };

    explicit ShapeShifterContainer (Direction _direction);
    virtual ~ShapeShifterContainer();

    Direction direction;
    OwnedArray<GapGrabber> grabbers;

    Array<ShapeShifter*> shifters;

    static constexpr int gap = 6;

    void resized() override;


    //Generic handling
    void insertShifterAt (ShapeShifter* shifter, int index, bool resizeAfter = true);
    void removeShifter (ShapeShifter* shifter, bool deleteShifter, bool silent = false, bool resizeAfter = true);

    ShapeShifterPanel* insertPanelAt (ShapeShifterPanel* panel, int index = -1, bool resizeAfter = true);
    ShapeShifterPanel* insertPanelRelative (ShapeShifterPanel* panel, ShapeShifterPanel* relativeTo, ShapeShifterPanel::AttachZone zone, bool resizeAfter = true);

    ShapeShifterContainer* insertContainerAt (ShapeShifterContainer* container, int index = -1, bool resizeAfter = true);

    void movePanelsInContainer (ShapeShifterPanel* newPanel, ShapeShifterPanel* containedPanel, Direction _newDir, bool secondBeforeFirst);

    bool isFlexible() override;

    void clear();

    virtual var getCurrentLayout() override;
    virtual void loadLayoutInternal (var layout) override;

    virtual void grabberGrabUpdate (GapGrabber* gg, int dist) override;
    virtual void panelDetach (ShapeShifterPanel*) override;
    virtual void panelEmptied (ShapeShifterPanel* panel) override;
    virtual void panelDestroyed (ShapeShifterPanel*) override;

    virtual void containerEmptied (ShapeShifterContainer*) override;
    virtual void oneShifterRemaining (ShapeShifterContainer* container, ShapeShifter* lastShifter) override;

    ListenerList<ShapeShifterContainerListener> containerListeners;
    void addShapeShifterContainerListener (ShapeShifterContainerListener* newListener) { containerListeners.add (newListener); }
    void removeShapeShifterContainerListener (ShapeShifterContainerListener* listener) { containerListeners.remove (listener); }

    int getPreferredWidth() override;
    int getPreferredHeight() override;
    int getMinWidth() override;
    int getMinHeight() override;

    void paint(Graphics & g)override;
    void paintOverChildren(Graphics & g) override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShapeShifterContainer)
    
};



