/*
  ==============================================================================

    Controllable.cpp
    Created: 8 Mar 2016 1:08:56pm
    Author:  bkupe

  ==============================================================================
*/

#include "Controllable.h"
#include "ControllableContainer.h"
#include "JsHelpers.h"

Controllable::Controllable(const Type &type, const String & niceName, const String &description, bool enabled) :
    type(type),
    description(description),
    parentContainer(nullptr),
    hasCustomShortName(false),
    isControllableExposed(true),
    isControllableFeedbackOnly(false),
    hideInEditor(false),
	replaceSlashesInShortName(true)
{
    setEnabled(enabled);
    setNiceName(niceName);
}

void Controllable::setNiceName(const String & _niceName) {
	if (niceName == _niceName) return;

	this->niceName = _niceName;
	if (!hasCustomShortName) setAutoShortName();
}

void Controllable::setCustomShortName(const String & _shortName)
{
	this->shortName = _shortName;
	hasCustomShortName = true;
}

void Controllable::setAutoShortName() {
	hasCustomShortName = false;
	shortName = StringUtil::toShortName(niceName, replaceSlashesInShortName);
	updateControlAddress();
}

void Controllable::setEnabled(bool value, bool silentSet, bool force)
{
	if (!force && value == enabled) return;

	enabled = value;
	if (!silentSet) listeners.call(&Listener::controllableStateChanged, this);
}

void Controllable::setParentContainer(ControllableContainer * container)
{
	this->parentContainer = container;
	updateControlAddress();
}

void Controllable::updateControlAddress()
{
	this->controlAddress = getControlAddress();
	listeners.call(&Listener::controllableControlAddressChanged, this);
}

String Controllable::getControlAddress(ControllableContainer * relativeTo)
{
    StringArray addressArray;
    addressArray.add(shortName);

    ControllableContainer * pc = parentContainer;

    while (pc != relativeTo)
    {
        if(!pc->skipControllableNameInAddress) addressArray.insert(0, pc->shortName);
        pc = pc->parentContainer;
    }

    return "/" + addressArray.joinIntoString("/");
}

DynamicObject * Controllable::createDynamicObject()
{
	DynamicObject* dObject = new DynamicObject();
	dObject->setProperty(ptrIdentifier, (int64)this);
	return dObject;
}


//STATIC 


var Controllable::setControllable(const juce::var::NativeFunctionArgs& a) {

	Controllable * c = getObjectPtrFromJS<Controllable>(a);
	bool success = false;

	if (c != nullptr)
	{

		success = true;

		if (a.numArguments == 0 && c->type == Controllable::Type::TRIGGER) {
			((Trigger *)c)->trigger();
		}

		else {
			var value = a.arguments[0];
			switch (c->type)
			{
			case Controllable::Type::TRIGGER:
				if (value.isBool() && (bool)value)
					((Trigger *)c)->trigger();
				else if ((value.isDouble() || value.isInt() || value.isInt64()) && (float)value>0)
					((Trigger *)c)->trigger();

				break;

			case Controllable::Type::BOOL:
				if (value.isBool())
					((BoolParameter *)c)->setValue((bool)value);
				break;

			case Controllable::Type::FLOAT:
				if (value.isDouble() || value.isInt() || value.isInt64())
					((FloatParameter *)c)->setValue(value);
				break;
			case Controllable::Type::INT:
				if (value.isInt() || value.isInt64())
					((IntParameter *)c)->setValue(value);
				break;


			case Controllable::Type::STRING:
				if (value.isString())
					((StringParameter *)c)->setValue(value);
				break;

			default:
				success = false;
				break;

			}
		}
	}


	return var();
}
