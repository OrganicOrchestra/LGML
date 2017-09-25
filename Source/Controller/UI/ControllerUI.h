/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#ifndef CONTROLLERUI_H_INCLUDED
#define CONTROLLERUI_H_INCLUDED


#include "../Controller.h"
#include "../../UI/Inspector/InspectableComponent.h"
#include "../../Controllable/Parameter/UI/TriggerBlinkUI.h"

class StringParameterUI;


class ControllerUI : public InspectableComponent, public ButtonListener
{
public:
    ControllerUI(Controller * controller);
    virtual ~ControllerUI();

	ScopedPointer<StringParameterUI> nameTF;
	ScopedPointer<ParameterUI> enabledBT;
	ImageButton removeBT;

	ScopedPointer<TriggerBlinkUI> activityBlink;

    Controller * controller;

    virtual void paint(Graphics &g) override;
    virtual void resized() override;

	virtual void mouseDown(const MouseEvent &e) override;
	virtual void buttonClicked(Button *) override;
	bool keyPressed(const KeyPress &e) override;

	virtual InspectorEditor * createEditor() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllerUI)
};


#endif  // CONTROLLERUI_H_INCLUDED
