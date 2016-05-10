/*
  ==============================================================================

    OSCControllerUI.cpp
    Created: 10 May 2016 12:08:42pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCControllerUI.h"
#include "OSCControllerEditor.h"

OSCControllerUI::OSCControllerUI(OSCController * controller) :
	ControllerUI(controller),
	oscController(controller)
{
}

OSCControllerUI::~OSCControllerUI()
{
}

InspectorEditor * OSCControllerUI::getEditor()
  {
	  return new OSCControllerEditor(this);
  }
