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
    jassert(controllable!=nullptr);
    updateTooltip();
    controllable->addControllableListener(this);
}

ControllableUI::~ControllableUI()
{
    if(controllable)controllable->removeControllableListener(this);
}

void ControllableUI::controllableStateChanged(Controllable * c)
{
    setAlpha(c->enabled ? 1 : .5f);
}

void ControllableUI::controllableControlAddressChanged(Controllable *)
{
    updateTooltip();
}

void ControllableUI::updateTooltip()
{
    tooltip = controllable->description + "\nControl Address : " + controllable->controlAddress;
    setTooltip(tooltip);
}
