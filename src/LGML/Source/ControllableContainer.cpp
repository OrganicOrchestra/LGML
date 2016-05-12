/*
 ==============================================================================

 ControllableContainer.cpp
 Created: 8 Mar 2016 1:15:36pm
 Author:  bkupe

 ==============================================================================
 */

#include "ControllableContainer.h"

#include "ControllableContainerEditor.h"
#include "ControllableUI.h"

ControllableContainer::ControllableContainer(const String & niceName) :
parentContainer(nullptr),
hasCustomShortName(false),
skipControllableNameInAddress(false),
currentPreset(nullptr)
{
    setNiceName(niceName);
}

ControllableContainer::~ControllableContainer()
{
    //controllables.clear();
    //DBG("CLEAR CONTROLLABLE CONTAINER");

    controllables.clear();
    masterReference.clear();
}

void ControllableContainer::addParameter(Parameter * p)
{
	addParameterInternal(p);
}

FloatParameter * ControllableContainer::addFloatParameter(const String & _niceName, const String & description, const float & initialValue, const float & minValue, const float & maxValue, const bool & enabled)
{
	String targetName = getUniqueNameInContainer(_niceName);
    FloatParameter * p = new FloatParameter(targetName, description, initialValue, minValue, maxValue, enabled);
    addParameterInternal(p);
    return p;
}

IntParameter * ControllableContainer::addIntParameter(const String & _niceName, const String & _description, const int & initialValue, const int & minValue, const int & maxValue, const bool & enabled)
{
	String targetName = getUniqueNameInContainer(_niceName);
    IntParameter * p = new IntParameter(targetName, _description, initialValue, minValue, maxValue, enabled);
    addParameterInternal(p);
    return p;
}

BoolParameter * ControllableContainer::addBoolParameter(const String & _niceName, const String & _description, const bool & value, const bool & enabled)
{
	String targetName = getUniqueNameInContainer(_niceName);
	BoolParameter * p = new BoolParameter(targetName, _description, value, enabled);
    addParameterInternal(p);
    return p;
}

StringParameter * ControllableContainer::addStringParameter(const String & _niceName, const String & _description, const String &value, const bool & enabled)
{
	String targetName = getUniqueNameInContainer(_niceName);
	StringParameter * p = new StringParameter(targetName, _description, value, enabled);
    addParameterInternal(p);
    return p;
}

Trigger * ControllableContainer::addTrigger(const String & _niceName, const String & _description, const bool & enabled)
{
	String targetName = getUniqueNameInContainer(_niceName);
	Trigger * t = new Trigger(targetName, _description, enabled);
    controllables.add(t);
    t->setParentContainer(this);
    t->addTriggerListener(this);

    controllableContainerListeners.call(&ControllableContainerListener::controllableAdded, t);
        notifyStructureChanged();
    return t;
}

void ControllableContainer::removeControllable(Controllable * c)
{
    controllableContainerListeners.call(&ControllableContainerListener::controllableRemoved, c);

    // @ben change nested callback for a special callback allowing listener to synchronize themselves without having to listen to every container

    controllables.removeObject(c);
    notifyStructureChanged();
}


void ControllableContainer::notifyStructureChanged(){

	controllableContainerListeners.call(&ControllableContainerListener::childStructureChanged, this);
}


void ControllableContainer::setNiceName(const String &_niceName) {
	if (niceName == _niceName) return;
    niceName = _niceName;
    if (!hasCustomShortName) setAutoShortName();
}

void ControllableContainer::setCustomShortName(const String &_shortName){
    shortName = _shortName;
    hasCustomShortName = true;
}

void ControllableContainer::setAutoShortName() {
	hasCustomShortName = false;
    shortName = StringUtil::toShortName(niceName);

    updateChildrenControlAddress();
}



Controllable * ControllableContainer::getControllableByName(const String & name, bool searchNiceNameToo)
{
    for (auto &c : controllables)
    {
        if (c->shortName == name || (searchNiceNameToo && c->niceName == name)) return c;
    }

    return nullptr;
}

void ControllableContainer::addChildControllableContainer(ControllableContainer * container)
{ 
	
    controllableContainers.add(container);
	container->addControllableContainerListener(this);
    container->setParentContainer(this);
	controllableContainerListeners.call(&ControllableContainerListener::controllableContainerAdded, container);
	notifyStructureChanged();
}

void ControllableContainer::removeChildControllableContainer(ControllableContainer * container)
{
	this->controllableContainers.removeAllInstancesOf(container);
	container->removeControllableContainerListener(this);
	controllableContainerListeners.call(&ControllableContainerListener::controllableContainerRemoved, container);
	notifyStructureChanged();
	container->setParentContainer(nullptr);
}

ControllableContainer * ControllableContainer::getControllableContainerByName(const String & name, bool searchNiceNameToo)
{
    for (auto &cc : controllableContainers)
    {
        if (cc->shortName == name || (searchNiceNameToo && cc->niceName == name)) return cc;
    }

    return nullptr;

}

void ControllableContainer::setParentContainer(ControllableContainer * container)
{
    this->parentContainer = container;
    for (auto &c : controllables) c->updateControlAddress();
    for (auto &cc : controllableContainers) cc->updateChildrenControlAddress();

}

void ControllableContainer::updateChildrenControlAddress()
{
    for (auto &c : controllables) c->updateControlAddress();
    for (auto &cc : controllableContainers) cc->updateChildrenControlAddress();

}

Array<Controllable*> ControllableContainer::getAllControllables(bool recursive,bool getNotExposed)
{
    Array<Controllable*> result;
    for (auto &c : controllables)
    {
        if (getNotExposed || c->isControllableExposed) result.add(c);
    }

    if (recursive)
    {
        for (auto &cc : controllableContainers) result.addArray(cc->getAllControllables(true,getNotExposed));
    }

    return result;
}

Array<Parameter*> ControllableContainer::getAllParameters(bool recursive, bool getNotExposed)
{
    Array<Parameter*> result;
    for (auto &c : controllables)
    {
        if (c->type == Controllable::Type::TRIGGER) continue;
        if (getNotExposed || c->isControllableExposed) result.add((Parameter *)c);
    }

    if (recursive)
    {
        for (auto &cc : controllableContainers) result.addArray(cc->getAllParameters(true, getNotExposed));
    }

    return result;
}



Controllable * ControllableContainer::getControllableForAddress(String address, bool recursive, bool getNotExposed)
{
    StringArray addrArray;
    addrArray.addTokens(address, juce::StringRef("/"), juce::StringRef("\""));
    addrArray.remove(0);

    return getControllableForAddress(addrArray, recursive, getNotExposed);
}

Controllable * ControllableContainer::getControllableForAddress(StringArray addressSplit, bool recursive, bool getNotExposed)
{
    if (addressSplit.size() == 0) jassertfalse; // SHOULD NEVER BE THERE !

    bool isTargetAControllable = addressSplit.size() == 1;

    if (isTargetAControllable)
    {
        //DBG("Check controllable Address : " + shortName);
        for (auto &c : controllables)
        {
            if (c->shortName == addressSplit[0])
            {
                //DBG(c->shortName);
                if (c->isControllableExposed || getNotExposed) return c;
                else return nullptr;
            }
        }

        //no found in direct children controllables, maybe in a skip container ?
        for (auto &cc : controllableContainers)
        {
            if (cc->skipControllableNameInAddress)
            {
                Controllable * tc = cc->getControllableByName(addressSplit[0]);

                if (tc != nullptr) return tc;
            }
        }
    }
    else
    {
        for (auto &cc : controllableContainers)
        {

            if (!cc->skipControllableNameInAddress)
            {
                if (cc->shortName == addressSplit[0])
                {
                    addressSplit.remove(0);
                    return cc->getControllableForAddress(addressSplit,recursive,getNotExposed);
                }
            }
            else
            {
                ControllableContainer * tc = cc->getControllableContainerByName(addressSplit[0]);
                if (tc != nullptr)
                {
                    addressSplit.remove(0);
                    return tc->getControllableForAddress(addressSplit,recursive,getNotExposed);
                }

            }
        }
    }

    return nullptr;
}


bool ControllableContainer::loadPreset(PresetManager::Preset * preset)
{
    if (preset == nullptr) return false;

    for (auto &pv : preset->presetValues)
    {
        Parameter * p = (Parameter *)getControllableForAddress(pv->paramControlAddress);
        if (p != nullptr) p->setValue(pv->presetValue);
    }

    currentPreset = preset;

    return true;
}

void ControllableContainer::saveNewPreset(const String & _name)
{
	PresetManager::Preset * pre = PresetManager::getInstance()->addPresetFromControllableContainer(_name, getPresetFilter(), this, true);
	loadPreset(pre);
}

bool ControllableContainer::saveCurrentPreset()
{
    if (currentPreset == nullptr) return false;

    for (auto &pv : currentPreset->presetValues)
    {
        Parameter * p = (Parameter *)getControllableForAddress(pv->paramControlAddress);
        if (p != nullptr)
        {
            pv->presetValue = p->value;
        }
    }

    return true;
}

bool ControllableContainer::resetFromPreset()
{
    if (currentPreset == nullptr) return false;

    for (auto &pv : currentPreset->presetValues)
    {
        Parameter * p = (Parameter *)getControllableForAddress(pv->paramControlAddress);
        if (p != nullptr) p->resetValue();
    }

    currentPreset = nullptr;
    return true;
}

var ControllableContainer::getPresetValueFor(Parameter * p)
{
	if (currentPreset == nullptr) return var();
	return currentPreset->getPresetValue(p->getControlAddress(this));
}




String ControllableContainer::getPresetFilter()
{
	return shortName;
}


void ControllableContainer::dispatchFeedback(Controllable * c)
{
    //    @ben removed else here to enable containerlistener call back of non root (proxies) is it overkill?
    if (parentContainer != nullptr){ parentContainer->dispatchFeedback(c); }
    controllableContainerListeners.call(&ControllableContainerListener::controllableFeedbackUpdate, c);

}



void ControllableContainer::parameterValueChanged(Parameter * p)
{
    onContainerParameterChanged(p);
    if (p->isControllableExposed) dispatchFeedback(p);
}

void ControllableContainer::triggerTriggered(Trigger * t)
{
    onContainerTriggerTriggered(t);
    if (t->isControllableExposed) dispatchFeedback(t);
}




void ControllableContainer::addParameterInternal(Parameter * p)
{
    p->setParentContainer(this);
    controllables.add(p);
    p->addParameterListener(this);
    controllableContainerListeners.call(&ControllableContainerListener::controllableAdded, p);
    notifyStructureChanged();
}





var ControllableContainer::getJSONData()
{
	var data(new DynamicObject());

	var paramsData;

	Array<Controllable *> cont = ControllableContainer::getAllControllables(true, true);

	for (auto &c : cont) {
		Parameter * base = dynamic_cast<Parameter*>(c);
		if (base)
		{
			var pData(new DynamicObject());
			pData.getDynamicObject()->setProperty("controlAddress", base->getControlAddress(this));
			pData.getDynamicObject()->setProperty("value", base->value);
			paramsData.append(pData);
		}
		else if (dynamic_cast<Trigger*>(c) != nullptr) {

		}
		else {
			// should never happen un less another Controllable type than parameter or trigger has been introduced
			jassertfalse;
		}
	}

	if (currentPreset != nullptr)
	{
		data.getDynamicObject()->setProperty("preset", currentPreset->name);
	}

	data.getDynamicObject()->setProperty("parameters", paramsData);

	return data;
}

void ControllableContainer::loadJSONData(var data)
{

	if (data.getDynamicObject()->hasProperty("preset"))
	{
		loadPreset(PresetManager::getInstance()->getPreset(getPresetFilter(), data.getProperty("preset",var())));
	}

	Array<var> * paramsData = data.getProperty("parameters", var()).getArray();

	for (var &pData : *paramsData)
	{
		String pControlAddress = pData.getProperty("controlAddress", var());

		Controllable * c = getControllableForAddress(pControlAddress, true, true);
		if (Parameter * p = dynamic_cast<Parameter*>(c)) {
			p->setValue(pData.getProperty("value", var()));
		}
		else {
			DBG("NodeBase::loadJSONData -> other Controllable than Parameters?");
			jassertfalse;
		}
	}

	loadJSONDataInternal(data);

	controllableContainerListeners.call(&ControllableContainerListener::controllableContainerPresetLoaded, this);
}

void ControllableContainer::childStructureChanged(ControllableContainer *)
{
	notifyStructureChanged();
}

String ControllableContainer::getUniqueNameInContainer(const String & sourceName, int suffix)
{
	String resultName = sourceName;
	if (suffix > 0) resultName += " " + String(suffix);

	if (getControllableByName(resultName,true) != nullptr)
	{
		return getUniqueNameInContainer(sourceName, suffix + 1);
	}
	if (getControllableContainerByName(resultName,true) != nullptr)
	{
		return getUniqueNameInContainer(sourceName, suffix + 1);
	}

	return resultName;
}
