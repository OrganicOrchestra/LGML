/*
  ==============================================================================

    SerialControllerUI.cpp
    Created: 22 May 2016 4:50:46pm
    Author:  bkupe

  ==============================================================================
*/

#include "SerialControllerUI.h"
#include "SerialControllerEditor.h"

SerialControllerUI::SerialControllerUI(SerialController * controller) :
	ControllerUI(controller),
	serialController(controller)
{
}

SerialControllerUI::~SerialControllerUI()
{
}

InspectorEditor * SerialControllerUI::getEditor()
  {
	  return new SerialControllerEditor(this);
  }
