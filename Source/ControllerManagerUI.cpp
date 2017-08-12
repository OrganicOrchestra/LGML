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


#include "ControllerManagerUI.h"

#include "Style.h"
#include "ControllerFactory.h"
#include "Inspector.h"
#include "ShapeShifterManager.h"

//==============================================================================
ControllerManagerUI::ControllerManagerUI(ControllerManager * manager):
    manager(manager)
{

    manager->addControllerListener(this);

	for (auto &c : manager->controllers)
	{
		addControllerUI(c);
	}

}

ControllerManagerUI::~ControllerManagerUI()
{
    manager->removeControllerListener(this);
	clear();
}

void ControllerManagerUI::clear()
{
	while(controllersUI.size() > 0)
	{
		removeControllerUI(controllersUI[0]->controller);
	}
}


void ControllerManagerUI::controllerAdded(Controller * c)
{
  MessageManager::getInstance()->callAsync([this,c](){addControllerUI(c);});
}

void ControllerManagerUI::controllerRemoved(Controller * c)
{

  MessageManager::getInstance()->callAsync([this,c](){ removeControllerUI(c);});

}

ControllerUI * ControllerManagerUI::addControllerUI(Controller * controller)
{
    if (getUIForController(controller) != nullptr)
    {
        DBG("Controller already exists");
        return nullptr;
    }

	ControllerUI * cui = controller->createUI();
    controllersUI.add(cui);

  {
    MessageManagerLock ml;
    addAndMakeVisible(cui);
    resized();

  }
	cui->selectThis();

  notifyParentViewPort();
    return cui;
}

void ControllerManagerUI::removeControllerUI(Controller * controller)
{
    ControllerUI * cui = getUIForController(controller);
    if (cui == nullptr)
    {
        DBG("Controller dont exist");
        return;
    }

  {
    MessageManagerLock ml;
    removeChildComponent(getIndexOfChildComponent(cui));
    controllersUI.removeObject(cui);

    resized();
  }
  notifyParentViewPort();

}

ControllerUI * ControllerManagerUI::getUIForController(Controller * controller)
{
    for (auto &cui : controllersUI)
    {
        if (cui->controller == controller) return cui;
    }

    return nullptr;
}

const int elemGap = 5;
const int elemHeight = 20;
void ControllerManagerUI::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(1);
    for (auto &cui : controllersUI)
    {
		cui->setBounds(r.removeFromTop(elemHeight));
		r.removeFromTop(elemGap);
    }
}

void ControllerManagerUI::paint (Graphics&)
{
    //ContourComponent::paint(g);
}

int ControllerManagerUI::getContentHeight(){
  return controllersUI.size() * (elemHeight+elemGap) + 2;

}

void ControllerManagerUI::mouseDown(const MouseEvent & event)
{
    if (event.eventComponent == this)
    {
        if (event.mods.isRightButtonDown())
        {

            ScopedPointer<PopupMenu> menu( new PopupMenu());
            ScopedPointer<PopupMenu> addNodeMenu( ControllerFactory::getControllerTypesMenu(0));
            menu->addSubMenu("Add Controller", *addNodeMenu);

            int result = menu->show();
            if (result >= 1 && result <= addNodeMenu->getNumItems())
            {
                manager->addController((ControllerFactory::ControllerType)(result - 1));
            }
		}
		else
		{
			if (Inspector::getInstanceWithoutCreating() != nullptr) Inspector::getInstance()->setCurrentComponent(nullptr);
		}
    }
}
