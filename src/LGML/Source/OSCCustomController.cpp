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
	OSCDirectController("OSC Custom Controller")
{
}

OSCCustomController::~OSCCustomController()
{
}

var OSCCustomController::getJSONData()
{
	var data = OSCController::getJSONData();

	
	return data;
}

void OSCCustomController::loadJSONData(var /*data*/)
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

	ControlVariable * v = getVariableForAddress(address);

	Result r(Result::ok());

	if (v == nullptr) r = Result::fail("Variable not found");

	if (msg.size() == 0) r = Result::fail("No argument");
	else if (!msg[0].isFloat32()) r = Result::fail("Argument is not a float");

	if (r != Result::ok())
	{
		return OSCDirectController::processMessageInternal(msg);
	}

	v->parameter->setValue(msg[0].getFloat32());

	return r;

}
