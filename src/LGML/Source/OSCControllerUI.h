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
class OSCController;
class OSCControllerUI : public ControllerUI
{
public:
	OSCControllerUI(OSCController * controller);
	virtual ~OSCControllerUI();

	OSCController * oscController;
	InspectorEditor * getEditor() override;
};



#endif  // OSCCONTROLLERUI_H_INCLUDED
