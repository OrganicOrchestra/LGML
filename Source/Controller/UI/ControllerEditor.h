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


#ifndef CONTROLLEREDITOR_H_INCLUDED
#define CONTROLLEREDITOR_H_INCLUDED

#include "ControllerUI.h" //keep
#include "../../Controllable/GenericControllableContainerEditor.h"

class ControllerEditor : public InspectorEditor,
						 public ButtonListener
{
public:
  ControllerEditor(Controller * controller,bool generateEditorFromParameters = true);
	virtual ~ControllerEditor();

	Controller * controller;

	virtual void resized() override;
	virtual int getContentHeight() override;

	//VARIABLES
	TextButton addParameterBT;
  ScopedPointer<GenericControllableContainerEditor> editor;
	void buttonClicked(Button * b) override;
  
};


#endif  // CONTROLLEREDITOR_H_INCLUDED
