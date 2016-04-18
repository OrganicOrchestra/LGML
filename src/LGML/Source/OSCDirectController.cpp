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

OSCDirectController::~OSCDirectController()
{
    NodeManager::getInstance()->removeControllableContainerListener(this);
}

void OSCDirectController::processMessage(const OSCMessage & msg)
{
    String addr = msg.getAddressPattern().toString();
    DBG("Process : " + addr);

    StringArray addrArray;
    addrArray.addTokens(addr,juce::StringRef("/"), juce::StringRef("\""));
    juce::Array<String> addSplit = addrArray.strings;

    addSplit.remove(0);
    String controller = addSplit[0];

    if (controller == "node")
    {
        addSplit.remove(0);
        Controllable * c = NodeManager::getInstance()->getControllableForAddress(addr);

        if (c != nullptr && !c->isControllableFeedbackOnly)
        {


            switch (c->type)
            {
            case Controllable::Type::TRIGGER:
                if(msg.size() == 0) ((Trigger *)c)->trigger();
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

void OSCDirectController::controllableAdded(Controllable *)
{
}

void OSCDirectController::controllableRemoved(Controllable *)
{

}

void OSCDirectController::controllableFeedbackUpdate(Controllable * c)
{
    DBG("Send OSC with address : " + c->controlAddress + " to " + remoteHostParam->value + ":" + remotePortParam->value);

#if JUCE_COMPILER_SUPPORTS_VARIADIC_TEMPLATES && JUCE_COMPILER_SUPPORTS_MOVE_SEMANTICS
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

#else
    OSCMessage msg(c->controlAddress);
    switch (c->type)
    {
        case Controllable::Type::TRIGGER:
            msg.addInt32(1);
            break;

        case Controllable::Type::BOOL:
            msg.addInt32(((BoolParameter *)c)->value?1:0);
            break;

        case Controllable::Type::FLOAT:
            msg.addFloat32(((FloatParameter *)c)->value);
            break;

        case Controllable::Type::INT:
            msg.addInt32(((IntParameter *)c)->value);
            break;

        case Controllable::Type::RANGE:
            DBG(" Martin @Ben : sending twice same value ??");
            jassertfalse;
            msg.addFloat32(((FloatParameter *)c)->value);
            msg.addFloat32(((FloatParameter *)c)->value);
            break;

        case Controllable::Type::STRING:
            msg.addString(((StringParameter *)c)->value);
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
