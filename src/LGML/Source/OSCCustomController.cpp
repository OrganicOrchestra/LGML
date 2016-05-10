/*
  ==============================================================================

    OSCCustomController.cpp
    Created: 10 May 2016 2:29:25pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCCustomController.h"
#include "OSCCustomControllerUI.h"

OSCCustomController::OSCCustomController() :
	OSCController("OSC Custom Controller")
{
}

OSCCustomController::~OSCCustomController()
{
}

ControllerUI * OSCCustomController::createUI()
{
	DBG("Create Custom UI");
	return new OSCCustomControllerUI(this);
}

Result OSCCustomController::processMessageInternal(const OSCMessage & msg)
{
	String address = msg.getAddressPattern().toString();
	DBG("Process message : " << address << " / " << msg.size() << "/" << String(msg[0].isFloat32()) );
	
	ControlVariable * v = getVariableForAddress(address);

	if (v == nullptr) return Result::fail("Variable not found");
	if (msg.size() == 0) return Result::fail("No argument");
	if (!msg[0].isFloat32()) return Result::fail("Argument is not a float");

	DBG("Passed  ! : " << address);

	v->parameter->setValue(msg[0].getFloat32());

	return Result::ok();

}
