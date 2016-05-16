/*
  ==============================================================================

    ControllableHelpers.cpp
    Created: 12 May 2016 4:21:18pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControllableHelpers.h"
#include "DebugHelpers.h"

ControllableContainerPopupMenu::ControllableContainerPopupMenu(ControllableContainer * rootContainer)
{
	populateMenu(this,rootContainer);
}

ControllableContainerPopupMenu::~ControllableContainerPopupMenu()
{
}

void ControllableContainerPopupMenu::populateMenu(PopupMenu * subMenu, ControllableContainer * container, int currentId)
{
	NLOG("CC PopupMenu","Populate menu " << container->niceName << " : " << currentId);
	for (auto &c : container->controllables)
	{
		LOG(" >> " << c->niceName);
		if (c->isControllableExposed) subMenu->addItem(currentId, c->niceName);
		currentId++;
	}

	for (auto &cc : container->controllableContainers)
	{
		LOG(" >> Container : " << cc->niceName);
		PopupMenu p;
		populateMenu(&p, cc, currentId);
		subMenu->addSubMenu(cc->niceName, p);
	}
}
