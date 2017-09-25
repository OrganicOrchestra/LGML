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


#ifndef MIDICONTROLLEREDITOR_H_INCLUDED
#define MIDICONTROLLEREDITOR_H_INCLUDED

#include "../../UI/ControllerEditor.h"
class MIDIControllerUI;
class MIDIController;
#include "../../../MIDI/MIDIListener.h"
#include "../../../MIDI/MIDIUIHelper.h"
#include "../../../Scripting/Js/JsEnvironmentUI.h"



class MIDIControllerEditor : public ControllerEditor, ComboBoxListener, MIDIListener::Listener
{
public:
	MIDIControllerEditor(MIDIController * controllerUI);
	virtual ~MIDIControllerEditor();

	MIDIController * midiController;
	ScopedPointer<JsEnvironmentUI> jsUI;

	MIDIDeviceChooser deviceChooser;
	ScopedPointer<NamedParameterUI> channelStepper;
	ScopedPointer<ParameterUI> incomingToogle;

	 

	void resized() override;
	void comboBoxChanged(ComboBox *) override;

	int getContentHeight() override;

	void currentDeviceChanged(MIDIListener *) override;
};


#endif  // MIDICONTROLLEREDITOR_H_INCLUDED
