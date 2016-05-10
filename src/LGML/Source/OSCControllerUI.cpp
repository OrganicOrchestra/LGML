/*
  ==============================================================================

    OSCControllerUI.cpp
    Created: 10 May 2016 12:08:42pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCControllerUI.h"

OSCControllerUI::OSCControllerUI(Controller * controller) :
	ControllerUI(controller),
	oscController((OSCController *)controller)
{
}

OSCControllerUI::~OSCControllerUI()
{
}

InspectorEditor * OSCControllerUI::getEditor()
  {
	  return new OSCControllerEditor(this);
  }
