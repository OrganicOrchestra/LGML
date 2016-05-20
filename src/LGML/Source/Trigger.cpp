/*
 ==============================================================================

 Trigger.cpp
 Created: 8 Mar 2016 1:09:29pm
 Author:  bkupe

 ==============================================================================
 */

#include "Trigger.h"

#include "TriggerButtonUI.h"
#include "TriggerBlinkUI.h"

#include "JsHelpers.h"

Trigger::Trigger(const String & niceName, const String &description, bool enabled) :
Controllable(TRIGGER, niceName, description, enabled),
isTriggering(false)
{
}

TriggerButtonUI * Trigger::createButtonUI()
{
    return new TriggerButtonUI(this);
}

TriggerBlinkUI * Trigger::createBlinkUI()
{
    return new TriggerBlinkUI(this);
}

ControllableUI * Trigger::createDefaultUI(){
    return createBlinkUI();
}

DynamicObject * Trigger::createDynamicObject()
{
	DynamicObject* dObject = Controllable::createDynamicObject();
	dObject->setMethod(jsTriggerIdentifier, setControllableValue);
	dObject->setProperty(jsPtrIdentifier, (int64)this);

	return dObject;
}
