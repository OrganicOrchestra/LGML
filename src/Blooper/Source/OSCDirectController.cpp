/*
  ==============================================================================

    OSCDirectController.cpp
    Created: 8 Mar 2016 10:27:37pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCDirectController.h"
#include "OSCDirectControllerContentUI.h"

OSCDirectController::OSCDirectController() :
	OSCController("OSC Direct Controller")
{

}

ControllerUI * OSCDirectController::createUI()
{
	return new ControllerUI(this, new OSCDirectControllerContentUI());
}
