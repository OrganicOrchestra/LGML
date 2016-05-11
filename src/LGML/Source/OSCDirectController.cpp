/*
  ==============================================================================

    OSCDirectController.cpp
    Created: 8 Mar 2016 10:27:37pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCDirectController.h"
#include "NodeManager.h"

OSCDirectController::OSCDirectController() :
    OSCController("OSC Direct Controller")
{
    NodeManager::getInstance()->addControllableContainerListener(this);
}

OSCDirectController::~OSCDirectController()
{
    NodeManager::getInstance()->removeControllableContainerListener(this);
}

Result OSCDirectController::processMessageInternal(const OSCMessage & msg)
{
	Result result = Result::ok();

     String addr = msg.getAddressPattern().toString();

    StringArray addrArray;
    addrArray.addTokens(addr,juce::StringRef("/"), juce::StringRef("\""));


    addrArray.remove(0);
    String controller = addrArray[0];


    if (controller == "node")
    {
        addrArray.remove(0);
        Controllable * c = NodeManager::getInstance()->getControllableForAddress(addrArray);


        if (c != nullptr)
        {
            if (!c->isControllableFeedbackOnly)
            {
				
                switch (c->type)
                {
                case Controllable::Type::TRIGGER:
                    if (msg.size() == 0) ((Trigger *)c)->trigger();
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
                        ((IntParameter *)c)->setValue((int)value); //normalized or not ? can user decide ?
                    }
                    break;

                case Controllable::Type::STRING:
                    ((Parameter *)c)->setValue(msg[0].getString());
                    break;

                default:
					result = Result::fail("Controllable type not handled");
					break;

                }
            }
        }
        else
        {
			result = Result::fail("Controllable not found");

            DBG("No Controllable for address : " + addr);
        }
	}
	else
	{
		result = Result::fail("address other than /node, not handler for now");
	}

	return result;
}

void OSCDirectController::controllableAdded(Controllable *)
{
}

void OSCDirectController::controllableRemoved(Controllable *)
{

}

void OSCDirectController::controllableFeedbackUpdate(Controllable * c)
{
    DBG("Send OSC with address : " + c->controlAddress + " to " + remoteHostParam->stringValue() + ":" + String(remotePortParam->stringValue().getIntValue()));

#if JUCE_COMPILER_SUPPORTS_VARIADIC_TEMPLATES && JUCE_COMPILER_SUPPORTS_MOVE_SEMANTICS
    switch (c->type)
    {
        case Controllable::Type::TRIGGER:
            sender.send(c->controlAddress,1);
            break;

        case Controllable::Type::BOOL:
            sender.send(c->controlAddress,((Parameter *)c)->intValue());
            break;

        case Controllable::Type::FLOAT:
            sender.send(c->controlAddress, ((Parameter *)c)->floatValue());
            break;

        case Controllable::Type::INT:
            sender.send(c->controlAddress, ((Parameter *)c)->intValue());
            break;

        case Controllable::Type::STRING:
            sender.send(c->controlAddress, ((Parameter *)c)->stringValue());
            break;
        default:
            DBG("OSC range param not supported");
            jassertfalse;
            break;
    }

#else

    OSCMessage msg(c->controlAddress);
    switch (c->type)
    {
        case Controllable::Type::TRIGGER:
            msg.addInt32(1);
            break;

        case Controllable::Type::BOOL:
            msg.addInt32(((Parameter *)c)->intValue());
            break;

        case Controllable::Type::FLOAT:
            msg.addFloat32(((Parameter *)c)->floatValue());
            break;

        case Controllable::Type::INT:
            msg.addInt32(((Parameter *)c)->intValue());
            break;

        case Controllable::Type::STRING:
            msg.addString(((Parameter *)c)->stringValue());
            break;

        default:
            DBG("OSC : unknown Controllable");
            jassertfalse;
            break;
    }

    sender.send(msg);

#endif


}

void OSCDirectController::controllableContainerAdded(ControllableContainer *)
{
}

void OSCDirectController::controllableContainerRemoved(ControllableContainer *)
{
}
