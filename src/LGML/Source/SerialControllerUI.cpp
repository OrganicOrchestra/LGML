/*
  ==============================================================================

    SerialControllerUI.cpp
    Created: 22 May 2016 4:50:46pm
    Author:  bkupe

  ==============================================================================
*/

#include "SerialControllerUI.h"
#include "SerialControllerEditor.h"
#include "TriggerBlinkUI.h"

SerialControllerUI::SerialControllerUI(SerialController * controller) :
	ControllerUI(controller),
	serialController(controller)
{
	activityBlink->animateIntensity = false;
}

SerialControllerUI::~SerialControllerUI()
{
}

InspectorEditor * SerialControllerUI::getEditor()
  {
	  return new SerialControllerEditor(this);
  }
