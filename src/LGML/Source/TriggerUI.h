/*
  ==============================================================================

    TriggerUI.h
    Created: 8 Mar 2016 3:48:52pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef TRIGGERUI_H_INCLUDED
#define TRIGGERUI_H_INCLUDED

#include "ControllableUI.h"
#include "Trigger.h"

class TriggerUI : public ControllableUI, public Trigger::Listener
{
public :
    TriggerUI(Trigger * trigger);
    virtual ~TriggerUI();

    Trigger*  trigger;
	bool showLabel;

    // Inherited via Listener
    virtual void triggerTriggered(Trigger * p) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriggerUI)


};


#endif  // TRIGGERUI_H_INCLUDED
