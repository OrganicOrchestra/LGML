/*
  ==============================================================================

    ControllerHeaderUI.h
    Created: 8 Mar 2016 10:48:47pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLERHEADERUI_H_INCLUDED
#define CONTROLLERHEADERUI_H_INCLUDED

#include "JuceHeader.h"
#include "ControllerUI.h"
#include "UIHelpers.h"

class BoolToggleUI;

class ControllerHeaderUI : public ContourComponent
{
public:
	ControllerHeaderUI();
	virtual ~ControllerHeaderUI();

	Controller * controller;
	ControllerUI * cui;

	Label titleLabel;
	ScopedPointer<BoolToggleUI> enabledUI;

	void setControllerAndUI(Controller * controller, ControllerUI * cui);
	virtual void init(); //override for proper init with controller and ui

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllerHeaderUI)
};


#endif  // CONTROLLERHEADERUI_H_INCLUDED
