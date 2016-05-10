/*
  ==============================================================================

    OSCControllerUI.h
    Created: 10 May 2016 12:08:41pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef OSCCONTROLLERUI_H_INCLUDED
#define OSCCONTROLLERUI_H_INCLUDED

#include "ControllerUI.h"
#include "OSCController.h"
#include "OSCControllerEditor.h"

class OSCControllerUI : public ControllerUI
{
public:
	OSCControllerUI(Controller * controller);
	virtual ~OSCControllerUI();

	OSCController * oscController;

	InspectorEditor * getEditor() override;
};



#endif  // OSCCONTROLLERUI_H_INCLUDED
