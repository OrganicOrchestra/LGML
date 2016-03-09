/*
  ==============================================================================

    ControllableUI.cpp
    Created: 9 Mar 2016 12:02:16pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControllableUI.h"

ControllableUI::ControllableUI(Controllable * controllable) :  
	controllable(controllable)
{
	setTooltip(controllable->description);
}

ControllableUI::~ControllableUI()
{
}
