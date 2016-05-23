/*
  ==============================================================================

    SerialControllerUI.h
    Created: 22 May 2016 4:50:46pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SERIALCONTROLLERUI_H_INCLUDED
#define SERIALCONTROLLERUI_H_INCLUDED

#include "ControllerUI.h"
#include "SerialController.h"

class SerialControllerUI : public ControllerUI
{
public:
	SerialControllerUI(SerialController * serialController);
	virtual ~SerialControllerUI();

	SerialController * serialController;

	InspectorEditor * getEditor() override;
};


#endif  // SERIALCONTROLLERUI_H_INCLUDED
