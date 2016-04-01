/*
 ==============================================================================
 
 ControllableContainer.cpp
 Created: 8 Mar 2016 1:15:36pm
 Author:  bkupe
 
 ==============================================================================
 */

#include "ControllableContainer.h"

#include "ControllableContainerEditor.h"

ControllableContainer::ControllableContainer(const String & niceName) :
niceName(niceName),
parentContainer(nullptr),
hasCustomShortName(false),
skipControllableNameInAddress(false)
{
    setNiceName(niceName);
	
}

ControllableContainer::~ControllableContainer()
{
    controllables.clear();
}

FloatParameter * ControllableContainer::addFloatParameter(const String & _niceName, const String & description, const float & initialValue, const float & minValue, const float & maxValue, const bool & enabled)
{
    if (getControllableByName(niceName) != nullptr)
    {
        DBG("ControllableContainer::add parameter, short Name already exists : " + niceName);
        return nullptr;
    }
	
    FloatParameter * p = new FloatParameter(_niceName, description, initialValue, minValue, maxValue, enabled);
    addParameterInternal(p);
    return p;
}

IntParameter * ControllableContainer::addIntParameter(const String & _niceName, const String & _description, const int & initialValue, const int & minValue, const int & maxValue, const bool & enabled)
{
    if (getControllableByName(_niceName) != nullptr)
    {
        DBG("ControllableContainer::add parameter, short Name already exists : " + _niceName);
        return nullptr;
    }
	
    IntParameter * p = new IntParameter(_niceName, _description, initialValue, minValue, maxValue, enabled);
    addParameterInternal(p);
    return p;
}

BoolParameter * ControllableContainer::addBoolParameter(const String & _niceName, const String & _description, const bool & value, const bool & enabled)
{
    if (getControllableByName(_niceName) != nullptr)
    {
        DBG("ControllableContainer::add parameter, short Name already exists : " + _niceName);
        return nullptr;
    }
	
    BoolParameter * p = new BoolParameter(_niceName, _description, value, enabled);
    addParameterInternal(p);
    return p;
}

StringParameter * ControllableContainer::addStringParameter(const String & _niceName, const String & _description, const String &value, const bool & enabled)
{
    if (getControllableByName(_niceName) != nullptr)
    {
        DBG("ControllableContainer::add parameter, short Name already exists : " + _niceName);
        return nullptr;
    }
	
    StringParameter * p = new StringParameter(_niceName, _description, value, enabled);
    addParameterInternal(p);
    return p;
}

Trigger * ControllableContainer::addTrigger(const String & _niceName, const String & _description, const bool & enabled)
{
    if (getControllableByName(_niceName) != nullptr)
    {
        DBG("ControllableContainer::add trigger, short Name already exists : " + _niceName);
        return nullptr;
    }
	
    Trigger * t = new Trigger(_niceName, _description, enabled);
    controllables.add(t);
    t->setParentContainer(this);
    t->addTriggerListener(this);
	
    controllableContainerListeners.call(&ControllableContainer::Listener::controllableAdded, t);
    return t;
}

void ControllableContainer::removeControllable(Controllable * c)
{
    controllableContainerListeners.call(&ControllableContainer::Listener::controllableRemoved, c);
    controllables.removeObject(c);
}

Controllable * ControllableContainer::getControllableByName(const String & name)
{
    for (auto &c : controllables)
    {
        if (c->shortName == name) return c;
    }
	
    return nullptr;
}

void ControllableContainer::addChildControllableContainer(ControllableContainer * container)
{
    controllableContainers.add(container);
    controllableContainerListeners.call(&ControllableContainer::Listener::controllableContainerAdded, container);
    container->setParentContainer(this);
}

void ControllableContainer::removeChildControllableContainer(ControllableContainer * container)
{
    container->setParentContainer(nullptr);
    controllableContainerListeners.call(&ControllableContainer::Listener::controllableContainerRemoved, container);
    controllableContainers.removeAllInstancesOf(container);
}

ControllableContainer * ControllableContainer::getControllableContainerByName(const String & name)
{
    for (auto &cc : controllableContainers)
    {
        if (cc->shortName == name) return cc;
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

Array<Controllable*> ControllableContainer::getAllControllables(bool recursive)
{
    Array<Controllable*> result;
    for (auto &c : controllables) if(c->isControllableExposed) result.add(c);
	
    if (recursive)
    {
        for (auto &cc : controllableContainers) result.addArray(cc->getAllControllables(true));
    }
	
    return result;
}


Controllable * ControllableContainer::getControllableForAddress(Array<String> addressSplit, bool , bool getNotExposed)
{
    if (addressSplit.size() == 0) jassertfalse; // SHOULD NEVER BE THERE !
	
    bool isTargetAControllable = addressSplit.size() == 1;
	
    if (isTargetAControllable)
    {
        DBG("Check controllable Address : " + shortName);
        for (auto &c : controllables)
        {
            if (c->shortName == addressSplit[0])
            {
                DBG(c->shortName);
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
                    return cc->getControllableForAddress(addressSplit);
                }
            }
            else
            {
                ControllableContainer * tc = cc->getControllableContainerByName(addressSplit[0]);
                if (tc != nullptr)
                {
                    addressSplit.remove(0);
                    return tc->getControllableForAddress(addressSplit);
                }
				
            }
        }
    }
	
    return nullptr;
}

void ControllableContainer::dispatchFeedback(Controllable * c)
{
	//    @ben removed else here to enable containerlistener call back of non root (proxies) is it overkill?
    if (parentContainer != nullptr){ parentContainer->dispatchFeedback(c);}
    controllableContainerListeners.call(&ControllableContainer::Listener::controllableFeedbackUpdate, c);
	
}



void ControllableContainer::parameterValueChanged(Parameter * p)
{
    if (p->isControllableExposed) dispatchFeedback(p);
}

void ControllableContainer::triggerTriggered(Trigger * t)
{
    if (t->isControllableExposed) dispatchFeedback(t);
}




void ControllableContainer::addParameterInternal(Parameter * p)
{
    p->setParentContainer(this);
    controllables.add(p);
    p->addParameterListener(this);
    controllableContainerListeners.call(&ControllableContainer::Listener::controllableAdded, p);
}


ControllableContainerEditor * ControllableContainer::createControllableContainerEditor(){
    ControllableContainerEditor * editor = new ControllableContainerEditor(this);
    Rectangle<int> bounds;
	
    int pad=3;
    int curY = pad;
    for(auto & c:controllables){
		if(c->isControllableExposed){
			ControllableUI * cUI = new NamedControllableUI(c->createDefaultControllableEditor(),100);
			cUI->setTopLeftPosition(0, curY);
			curY+=cUI->getHeight() + pad;
			editor->addControlUI(cUI);
			bounds = bounds.getUnion(cUI->getBounds().expanded(0,pad));
		}
    }
	
	
    for(auto &c:controllableContainers){
        ControllableContainerEditor * cE=c->createControllableContainerEditor();
        cE->setTopLeftPosition(0, curY);
        curY+=cE->getHeight()+pad;
        editor->addAndMakeVisible(cE);
        bounds = bounds.getUnion(cE->getBounds().expanded(0,pad));
    }
	
    editor->setBounds(bounds);
    return editor;
	
}
