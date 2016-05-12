/*
  ==============================================================================

    ControllableHelpers.cpp
    Created: 12 May 2016 4:21:18pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControllableHelpers.h"

ControllableContainerPopupMenu::ControllableContainerPopupMenu(ControllableContainer * rootContainer)
{
	populateMenu(this,rootContainer);
}

ControllableContainerPopupMenu::~ControllableContainerPopupMenu()
{
}

void ControllableContainerPopupMenu::populateMenu(PopupMenu * subMenu, ControllableContainer * container, int currentId)
{
	for (auto &c : container->controllables)
	{
		if (c->isControllableExposed) subMenu->addItem(currentId, c->niceName);
		currentId++;
	}

	for (auto &cc : container->controllableContainers)
	{
		PopupMenu p;
		populateMenu(&p, cc, currentId);
		subMenu->addSubMenu(cc->niceName, p);
	}
}
