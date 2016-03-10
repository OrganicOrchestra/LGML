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

	NodeManager::getInstance()->addControllableContainerListener(this);
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

		if (c != nullptr && !c->isControllableFeedbackOnly)
		{


			switch (c->type)
			{
			case Controllable::Type::TRIGGER:
				if(msg.size() == 0)	((Trigger *)c)->trigger();
				else if (msg[0].isInt32() || msg[0].isFloat32())
				{
					float val = msg[0].isInt32() ? msg[0].getInt32() : msg[0].getFloat32();
					if (val > 0) ((Trigger *)c)->trigger();
				}
				break;

			case Controllable::Type::BOOL:
				if (msg.size() > 0 && (msg[0].isInt32() || msg[0].isFloat32()))
				{
					float val = msg[0].isInt32() ? msg[0].getInt32() : msg[0].getFloat32();
					((BoolParameter *)c)->setValue(val > 0);
				}
				break;

			case Controllable::Type::FLOAT:
				if (msg.size() > 0 && (msg[0].isInt32() || msg[0].isFloat32()))
				{
					float value = msg[0].isInt32() ? msg[0].getInt32() : msg[0].getFloat32();
					((FloatParameter *)c)->setNormalizedValue(value); //normalized or not ? can user decide ?
				}
				break;

			case Controllable::Type::INT:
				if (msg.size() > 0 && (msg[0].isInt32() || msg[0].isFloat32()))
				{
					float value = msg[0].isInt32() ? msg[0].getInt32() : msg[0].getFloat32();
					((IntParameter *)c)->setValue(value); //normalized or not ? can user decide ?
				}
				break;

			case Controllable::Type::RANGE:
				if (msg.size() > 1 && (msg[0].isInt32() || msg[0].isFloat32()) && (msg[1].isInt32() || msg[1].isFloat32()))
				{
					float value1 = msg[0].isInt32() ? msg[0].getInt32() : msg[0].getFloat32();
					float value2 = msg[1].isInt32() ? msg[1].getInt32() : msg[1].getFloat32();
					((FloatRangeParameter *)c)->setValuesMinMax(value1, value2); 
				}
				break;

			case Controllable::Type::STRING:
				break;
			}
		}
		else
		{
			DBG("No Controllable for address : " + addr);
		}
	}
}

ControllerUI * OSCDirectController::createUI()
{
	return new ControllerUI(this, new OSCDirectControllerContentUI());
}

void OSCDirectController::controllableAdded(Controllable * c)
{
}

void OSCDirectController::controllableRemoved(Controllable * c)
{

}

void OSCDirectController::controllableFeedbackUpdate(Controllable * c)
{
	switch (c->type)
	{
		case Controllable::Type::TRIGGER:
			sender.send(c->controlAddress,1);
			break;

		case Controllable::Type::BOOL:
			sender.send(c->controlAddress,((BoolParameter *)c)->value?1:0);
			break;

		case Controllable::Type::FLOAT:
			sender.send(c->controlAddress, ((FloatParameter *)c)->value);
			break;

		case Controllable::Type::INT:
			sender.send(c->controlAddress, ((IntParameter *)c)->value);
			break;

		case Controllable::Type::RANGE:
			sender.send(c->controlAddress, ((FloatParameter *)c)->value, ((FloatParameter *)c)->value);
			break;

		case Controllable::Type::STRING:
			sender.send(c->controlAddress, ((StringParameter *)c)->value);
			break;
	}
}
