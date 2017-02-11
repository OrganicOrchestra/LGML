/*
  ==============================================================================

    OSCControllerUI.cpp
    Created: 10 May 2016 12:08:42pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCControllerUI.h"
#include "OSCControllerEditor.h"
#include "GenericControllableContainerEditor.h"

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
	  auto ui =  new GenericControllableContainerEditor(this);
    return ui;
  }
