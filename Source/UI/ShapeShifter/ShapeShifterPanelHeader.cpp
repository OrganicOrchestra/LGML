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

#if !ENGINE_HEADLESS

#include "ShapeShifterPanelHeader.h"
#include "../Style.h"
#include "ShapeShifterFactory.h"
#include "ShapeShifterPanel.h"

ShapeShifterPanelHeader::ShapeShifterPanelHeader()
{
    addMouseListener(this, true);
    //  addPannelMenu.setButtonText("+");
    addAndMakeVisible(addPannelMenu);
    //  addPannelMenu.setConnectedEdges(0);
    addPannelMenu.setTooltip(juce::translate("add tab to this panel"));
    addPannelMenu.addListener(this);
    setOpaque(true);
    setPaintingIsUnclipped(true);
}

ShapeShifterPanelHeader::~ShapeShifterPanelHeader()
{
}

void ShapeShifterPanelHeader::addTab(ShapeShifterContent *content)
{
    ShapeShifterPanelTab *tab = new ShapeShifterPanelTab(content);
    LGMLUIUtils::markHasNewBackground(tab, 1);
    attachTab(tab);
}

void ShapeShifterPanelHeader::removeTab(ShapeShifterPanelTab *tab, bool doRemove)
{
    tab->removeShapeShifterTabListener(this);
    removeChildComponent(tab);
    tabs.removeObject(tab, doRemove);
    resized();
}

void ShapeShifterPanelHeader::attachTab(ShapeShifterPanelTab *tab)
{
    tab->addShapeShifterTabListener(this);
    addAndMakeVisible(tab);
    tab->setMouseCursor(MouseCursor::UpDownLeftRightResizeCursor);
    tabs.add(tab);
    resized();
}

ShapeShifterPanelTab *ShapeShifterPanelHeader::getTabForContent(ShapeShifterContent *content)
{
    for (auto &t : tabs)
    {
        if (t->content == content)
            return t;
    }

    return nullptr;
}

void ShapeShifterPanelHeader::mouseDown(const MouseEvent &e)
{
    ShapeShifterPanelTab *tab = dynamic_cast<ShapeShifterPanelTab *>(e.eventComponent);

    if (tab != nullptr)
    {
        listeners.call(&Listener::tabSelect, tab);
    }
}

void ShapeShifterPanelHeader::mouseDrag(const MouseEvent &e)
{
    int minDetachDistanceY = 10;

    if (e.eventComponent == this)
    {
        if (abs(e.getDistanceFromDragStartY()) > minDetachDistanceY)
        {
            //            Component::BailOutChecker checker (this);
            //            listeners.callChecked (checker,&Listener::headerDrag);// check its not self deleting
            listeners.call(&Listener::headerDrag);
        }
    }
    else
    {
        ShapeShifterPanelTab *tab = dynamic_cast<ShapeShifterPanelTab *>(e.eventComponent);

        if ((abs(e.getDistanceFromDragStartY()) > minDetachDistanceY) && tab != nullptr)
        {
            // a bug seem to appear randomly here (one of listeners gets corrupted)
            // TODO check self deletion
            //            Component::BailOutChecker checker (this);
            //            listeners.callChecked (checker,&Listener::tabDrag, tab); // check its not self deleting
            listeners.call(&Listener::tabDrag, tab);
        }
    }
}

void ShapeShifterPanelHeader::paint(Graphics &g)
{
    LGMLUIUtils::fillBackground(this, g);
}

void ShapeShifterPanelHeader::resized()
{
    //re arrange tabs
    Rectangle<int> r = getLocalBounds();

    if (r.getWidth() == 0 || r.getHeight() == 0)
        return;

    for (auto &t : tabs)
    {
        t->setBounds(r.removeFromLeft(jmin<int>(getWidth(), t->getLabelWidth())));
    }
    r.removeFromLeft(2);
    const int buttonSize = r.getHeight() - 2;
    // stick to right if to much pannels
    if (r.getWidth() < buttonSize)
    {
        addPannelMenu.setBounds(r.removeFromRight(buttonSize).withSizeKeepingCentre(buttonSize, buttonSize));
    }
    else
    {
        addPannelMenu.setBounds(r.removeFromLeft(buttonSize).withSizeKeepingCentre(buttonSize, buttonSize));
    }
}

void ShapeShifterPanelHeader::askForRemoveTab(ShapeShifterPanelTab *tab)
{
    listeners.call(&Listener::askForRemoveTab, tab);
}

void ShapeShifterPanelHeader::buttonClicked(Button *b)
{
    if (b == &addPannelMenu)
    {
        PopupMenu menu;
        int currentID = 1;

        for (auto &n : globalPanelNames)
        {
            menu.addItem(currentID, juce::translate(n), true);
            currentID++;
        }

        int resID = menu.showMenu(PopupMenu::Options().withTargetComponent(&addPannelMenu));

        if (resID > 0)
        {
            String nameToAdd(globalPanelNames[resID - 1]);
            auto content = ShapeShifterFactory::createContentForName(nameToAdd);

            if (ShapeShifterPanel *parent = dynamic_cast<ShapeShifterPanel *>(getParentComponent()))
            {
                parent->addContent(content);
            }
        }
    }
}

#endif
