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


#ifndef CONTROLLABLEEDITOR_H_INCLUDED
#define CONTROLLABLEEDITOR_H_INCLUDED

#include "InspectorEditor.h"
class Controllable;
class ControllableUI;

class ControllableEditor : 
	public InspectorEditor
{
public:
	ControllableEditor(InspectableComponent * sourceComponent, Controllable * controllable);

	Controllable * controllable;
	
	Label label;
	ScopedPointer<ControllableUI> ui;

	void resized() override;

	int getContentHeight() override;

};



#endif  // CONTROLLABLEEDITOR_H_INCLUDED
