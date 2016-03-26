/*
  ==============================================================================

    TriggerUI.cpp
    Created: 8 Mar 2016 3:48:52pm
    Author:  bkupe

  ==============================================================================
*/

#include "TriggerUI.h"

TriggerUI::TriggerUI(Trigger * trigger) :
    ControllableUI(trigger),
    trigger(trigger)
{
    trigger->addTriggerListener(this);
}

TriggerUI::~TriggerUI()
{
    // TODO check global comportement for listeners
    //trigger->removeListener(this);
}

void TriggerUI::triggerTriggered(Trigger * p)
{
}
