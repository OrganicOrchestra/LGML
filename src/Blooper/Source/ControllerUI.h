/*
  ==============================================================================

    ControllerUI.h
    Created: 8 Mar 2016 10:46:01pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLERUI_H_INCLUDED
#define CONTROLLERUI_H_INCLUDED

#include "Controller.h"
class ControllerHeaderUI;
class ControllerContentUI;

class ControllerUI : public Component
{
public:
	ControllerUI(Controller * controller, ControllerContentUI * contentUI = nullptr, ControllerHeaderUI * headerUI = nullptr);
	virtual ~ControllerUI();

	ScopedPointer<ControllerHeaderUI> headerUI;
	ScopedPointer<ControllerContentUI> contentUI;

	Controller * controller;

	void paint(Graphics &g);
	void resized() override;

	void mouseDown(const MouseEvent &e) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllerUI)
};


#endif  // CONTROLLERUI_H_INCLUDED
