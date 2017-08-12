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


#ifndef SERIALCONTROLLEREDITOR_H_INCLUDED
#define SERIALCONTROLLEREDITOR_H_INCLUDED


#include "ControllerEditor.h"
#include "SerialUIHelper.h"
#include "JsEnvironmentUI.h"
#include "SerialController.h"//keep

class SerialControllerEditor :
	public ControllerEditor, ComboBoxListener,
	public SerialController::SerialControllerListener
{
public:
	SerialControllerEditor(Controller * controllerUI);
	virtual ~SerialControllerEditor();

	SerialDeviceChooser deviceChooser;

	SerialController * serialController;
	TextButton connectPortBT;

	ScopedPointer<JsEnvironmentUI> jsUI;
	ScopedPointer<ParameterUI> incomingToogle;

	OwnedArray<ParameterUI> variablesUI;

	int getContentHeight() override;


	void paint(Graphics &g) override;
	void resized() override;

	void updateConnectBTAndIndic();

	void comboBoxChanged(ComboBox *) override;
	void buttonClicked(Button *) override;

	void portOpened() override;
	void portClosed() override;
	void currentPortChanged() override;

};


#endif  // SERIALCONTROLLEREDITOR_H_INCLUDED
