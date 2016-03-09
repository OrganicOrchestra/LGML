/*
  ==============================================================================

    OSCDirectController.cpp
    Created: 8 Mar 2016 10:27:37pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCDirectController.h"
#include "OSCDirectControllerContentUI.h"
#include "NodeManager.h"

OSCDirectController::OSCDirectController() :
	OSCController("OSC Direct Controller")
{
	DBG("direct controller constructor");
}

void OSCDirectController::processMessage(const OSCMessage & msg)
{
	DBG("Process message in direct controller");
	String addr = msg.getAddressPattern().toString();
	StringArray addrArray;
	addrArray.addTokens(addr,juce::StringRef("/"), juce::StringRef("\""));
	juce::Array<String> addSplit = addrArray.strings;
	DBG(addrArray.strings.size());

	addSplit.remove(0);
	String controller = addSplit[0];
	DBG("Target Controller :" + controller);

	if (controller == "node")
	{
		addSplit.remove(0);
		Controllable * c = NodeManager::getInstance()->getControllableForAddress(addSplit);
		if (c == nullptr)
		{
			DBG("No Controllable for address : " + addr);
		}
		else
		{
			DBG("Found controller !");
			if (msg.size() == 1)
			{
				c->parseValueFromController(msg[0].isInt32()?msg[0].getInt32():msg[0].getFloat32());
			}
		}
	}
}

ControllerUI * OSCDirectController::createUI()
{
	return new ControllerUI(this, new OSCDirectControllerContentUI());
}
