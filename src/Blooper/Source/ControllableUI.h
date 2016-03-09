/*
  ==============================================================================

    ControllableUI.h
    Created: 9 Mar 2016 12:02:16pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLABLEUI_H_INCLUDED
#define CONTROLLABLEUI_H_INCLUDED

#include "JuceHeader.h"
#include "Controllable.h"

class ControllableUI : public Component, public SettableTooltipClient
{
public:
	ControllableUI(Controllable * controllable);
	virtual ~ControllableUI();

	Controllable * controllable;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllableUI)
};



#endif  // CONTROLLABLEUI_H_INCLUDED
