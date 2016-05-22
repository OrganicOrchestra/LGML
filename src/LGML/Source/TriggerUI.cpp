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
    trigger(trigger),
	showLabel(true)
{
    trigger->addTriggerListener(this);
}

TriggerUI::~TriggerUI()
{

    if(controllable.get())trigger->removeTriggerListener(this);
}

void TriggerUI::triggerTriggered(Trigger *)
{
}
