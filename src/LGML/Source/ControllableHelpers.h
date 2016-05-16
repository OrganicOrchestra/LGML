/*
  ==============================================================================

    ControllableHelpers.h
    Created: 12 May 2016 4:21:18pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLABLEHELPERS_H_INCLUDED
#define CONTROLLABLEHELPERS_H_INCLUDED

#include "JuceHeader.h"
#include "ControllableContainer.h"

class ControllableContainerPopupMenu : public PopupMenu
{
public:
	ControllableContainerPopupMenu(ControllableContainer * rootContainer);
	virtual ~ControllableContainerPopupMenu();

	void populateMenu(PopupMenu *subMenu, ControllableContainer * container, int currentId = 1);
};


#endif  // CONTROLLABLEHELPERS_H_INCLUDED
