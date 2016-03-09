/*
  ==============================================================================

    OSCController.cpp
    Created: 2 Mar 2016 8:50:08pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCController.h"

OSCController::OSCController(const String &name) :
	Controller(name)
  {
	  localPortParam = addStringParameter("Local Port", "The port to bind for the controller to receive OSC from it","11000");
	  remotePortParam = addStringParameter("Remote Port", "The port bound by the controller to send OSC to it","11001");
  }

  OSCController::~OSCController()
  {

  }
