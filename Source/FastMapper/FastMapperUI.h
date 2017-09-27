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

#ifndef FASTMAPPERUI_H_INCLUDED
#define FASTMAPPERUI_H_INCLUDED

#include "../UI/ShapeShifter/ShapeShifterContent.h"
#include "FastMapUI.h"
#include "../UI/Inspector/Inspector.h"
#include "../UI/Style.h"

class FastMapper;
class FastMapperUI;
class FastMapperUIListener
{
public:
    virtual ~FastMapperUIListener() {}
    virtual void fastMapperContentChanged (FastMapperUI*) {}
};

class FastMapperUI :
    public juce::Component,
    private ControllableContainerListener,
    private ButtonListener,
    private Inspector::InspectorListener
{
public:
    FastMapperUI (FastMapper* fastMapper, ControllableContainer* viewFilterContainer = nullptr);
    virtual ~FastMapperUI();

    FastMapper* fastMapper;
    TextButton linkToSelection;

    ScopedPointer<ParameterUI> autoAddBt;
    ScopedPointer<Component> potentialIn, potentialOut;
    OwnedArray<FastMapUI> mapsUI;

    ControllableContainer* viewFilterContainer;

    void clear();

    void addFastMapUI (FastMap*);
    void removeFastMapUI (FastMapUI*);

    void resetAndUpdateView();
    void setViewFilter (ControllableContainer* filterContainer);
    bool mapPassViewFilter (FastMap*);

    FastMapUI* getUIForFastMap (FastMap*);

    const int mapHeight = 35;
    const int gap = 5;
    int getContentHeight();

    void resized() override;
    void mouseDown (const MouseEvent& e) override;

    virtual void controllableContainerAdded (ControllableContainer*, ControllableContainer*) override;
    virtual void controllableContainerRemoved (ControllableContainer*, ControllableContainer*) override;

    ListenerList<FastMapperUIListener> fastMapperUIListeners;
    void addFastMapperUIListener (FastMapperUIListener* newListener) { fastMapperUIListeners.add (newListener); }
    void removeFastMapperUIListener (FastMapperUIListener* listener) { fastMapperUIListeners.remove (listener); }
private:

    void buttonClicked (Button*) override;
    void currentComponentChanged (Inspector* ) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FastMapperUI)

};

class FastMapperViewport :
    public ShapeShifterContentComponent,
    public FastMapperUIListener,
    private ButtonListener
{
public:
    FastMapperViewport (const String& contentName, FastMapperUI* _fastMapperUI) :
        fastMapperUI (_fastMapperUI),
        ShapeShifterContentComponent (contentName)
    {
        vp.setViewedComponent (fastMapperUI, true);
        vp.setScrollBarsShown (true, false);
        vp.setScrollOnDragEnabled (false);
        addAndMakeVisible (vp);
        addAndMakeVisible (addFastMapButton);
        addFastMapButton.addListener (this);
        addFastMapButton.setTooltip ("Add FastMap");
        vp.setScrollBarThickness (10);
        contentIsFlexible = true;
        fastMapperUI->addFastMapperUIListener (this);
    }

    virtual ~FastMapperViewport()
    {
        fastMapperUI->removeFastMapperUIListener (this);
    }


    void resized() override
    {
        vp.setBounds (getLocalBounds());
        int th = jmax<int> (fastMapperUI->getContentHeight(), getHeight());
        Rectangle<int> targetBounds = getLocalBounds().withPosition (fastMapperUI->getPosition()).withHeight (th);
        targetBounds.removeFromRight (vp.getScrollBarThickness());
        fastMapperUI->setBounds (targetBounds);
        addFastMapButton.setFromParentBounds (getLocalBounds());
    }

    void fastMapperContentChanged (FastMapperUI*)override
    {
        resized();
    }

    void buttonClicked (Button* b)override;


    Viewport vp;
    FastMapperUI* fastMapperUI;
    AddElementButton addFastMapButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FastMapperViewport)
};


#endif  // FASTMAPPERUI_H_INCLUDED
