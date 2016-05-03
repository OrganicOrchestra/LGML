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
niceName(niceName),
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
}

FloatParameter * ControllableContainer::addFloatParameter(const String & _niceName, const String & description, const float & initialValue, const float & minValue, const float & maxValue, const bool & enabled)
{
    if (getControllableByName(niceName) != nullptr)
    {
        DBG("ControllableContainer::add parameter, short Name already exists : " + niceName);
        jassertfalse;
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
    // @ben remove nested callback as it's not needed anymore for ControllableContainerSync
    // we may implement a special callback structure changed that triggered only by the root parent

//    ControllableContainer * notified = parentContainer;
//    while(notified!=nullptr){
//        notified->controllableContainerListeners.call(&ControllableContainer::Listener::controllableRemoved, c);
//        notified = notified->parentContainer;
//    }
    controllables.removeObject(c);
}



void ControllableContainer::setNiceName(const String &_niceName) {
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
//    ControllableContainer * notified = parentContainer;
//    while(notified!=nullptr){
//        notified->controllableContainerListeners.call(&ControllableContainer::Listener::controllableContainerAdded, container);
//        notified = notified->parentContainer;
//    }
}

void ControllableContainer::removeChildControllableContainer(ControllableContainer * container)
{
//    ControllableContainer * notified = parentContainer;
//    while(notified!=nullptr){
//        notified->controllableContainerListeners.call(&ControllableContainer::Listener::controllableContainerRemoved, container);
//        notified = notified->parentContainer;
//    }

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
    juce::Array<String> addSplit = addrArray.strings;
    addSplit.remove(0);

    return getControllableForAddress(addSplit, recursive, getNotExposed);
}

Controllable * ControllableContainer::getControllableForAddress(Array<String> addressSplit, bool recursive, bool getNotExposed)
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
    DBG("load preset, " << String(currentPreset != nullptr));
    if (preset == nullptr) return false;

    for (auto &pv : preset->presetValues)
    {
        Parameter * p = (Parameter *)getControllableForAddress(pv->paramControlAddress);
        if (p != nullptr) p->setValue(pv->presetValue);
    }

    currentPreset = preset;

    return true;
}

bool ControllableContainer::saveCurrentPreset()
{
    DBG("save current preset, " << String(currentPreset != nullptr));
    if (currentPreset == nullptr) return false;

    for (auto &pv : currentPreset->presetValues)
    {
        Parameter * p = (Parameter *)getControllableForAddress(pv->paramControlAddress);
        if (p != nullptr)
        {
            DBG("set preset value : " << p->niceName << " > " << p->stringValue());
            pv->presetValue = p->value;
        }
    }

    return true;
}

bool ControllableContainer::resetFromPreset()
{
    DBG("Reset from preset, " << String(currentPreset != nullptr));
    if (currentPreset == nullptr) return false;

    for (auto &pv : currentPreset->presetValues)
    {
        DBG("Reset, check presetValue " << pv->paramControlAddress << " > " << pv->presetValue.toString());
        Parameter * p = (Parameter *)getControllableForAddress(pv->paramControlAddress);
        if (p != nullptr) p->resetValue();
    }

    currentPreset = nullptr;
    return true;
}

void ControllableContainer::dispatchFeedback(Controllable * c)
{
    //    @ben removed else here to enable containerlistener call back of non root (proxies) is it overkill?
    if (parentContainer != nullptr){ parentContainer->dispatchFeedback(c); }
    controllableContainerListeners.call(&ControllableContainer::Listener::controllableFeedbackUpdate, c);

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
    controllableContainerListeners.call(&ControllableContainer::Listener::controllableAdded, p);
//    ControllableContainer * notified = parentContainer;
//    while(notified!=nullptr){
//        notified->controllableContainerListeners.call(&ControllableContainer::Listener::controllableAdded, p);
//        notified = notified->parentContainer;
//    }
}


Component * ControllableContainer::createControllableContainerEditor(Component * reference){
    ControllableContainerEditor * editor;
    if(reference==nullptr){editor= new ControllableContainerEditor(this,nullptr);}
    else{editor = (ControllableContainerEditor*)reference;}
    jassert(editor!=nullptr);

//    DBG("=====Editor : "+editor->owner->niceName);
    int idx = 0;
    // tries to create only new 
    Array<Controllable*> currentControllables;
    for(auto &c:controllables){currentControllables.add(c);}

    for(auto & c:currentControllables){
        if(c->hideInEditor)continue;

        ControllableUI * cUI =nullptr;
        if( idx < editor->controllableUIs.size()){
            ControllableUI* current = editor->controllableUIs.getUnchecked(idx);
            if(current && current->controllable.get() && current->controllable == c){
//                DBG("===Exist : "+current->controllable->niceName);
                cUI = current;
            }
            else{
                jassert(reference!=nullptr);
//                DBG("===Remove : "+current->getName());
                reference->removeChildComponent(current);
            }
        }
        if(cUI==nullptr){
            cUI= new NamedControllableUI(c->createControllableContainerEditor(),100);
//            DBG("===Create: "+cUI->controllable->niceName);
            editor->addControlUI(cUI);
        }


        idx++;
    }

    // delete if there is more
    Array<ControllableUI*> oldControllables;
    for(auto &c:editor->controllableUIs){oldControllables.add(c);}
    for(int i = idx ; i <oldControllables.size() ; i++  ){
        ControllableUI *c = oldControllables[i];
//        DBG("===Remove : " << c->getName());
        editor->removeChildComponent(c);
        editor->controllableUIs.removeAllInstancesOf(c);
        delete c;
    }

    idx =0;
        Array<ControllableContainer*> currentContainers;
    for(auto &c:controllableContainers){currentContainers.add(c);}
    for(auto &c:currentContainers){
        ControllableContainerEditor * cE=nullptr;
        if(idx < editor->editors.size()){

            if(c == editor->editors.getUnchecked(idx)->owner){
                cE = editor->editors.getUnchecked(idx);
            }

            else{
                editor->removeChildComponent(cE);
                editor->editors.removeAllInstancesOf(cE);
                delete cE;
            }
        }
        if(cE==nullptr){
            cE=(ControllableContainerEditor*)c->createControllableContainerEditor(nullptr);
            editor->editors.add(cE);
            editor->addAndMakeVisible(cE);

        }
        else{
            cE = (ControllableContainerEditor*)c->createControllableContainerEditor(cE);
        }

        idx++;
    }

    // delete if there is more
    Array<ControllableContainerEditor*> oldContainers;
    for(auto &c:editor->editors){oldContainers.add(c);}
    for(int i = idx ; i <oldContainers.size() ; i++  ){
        ControllableContainerEditor *c = oldContainers[i];
//        DBG("===Remove Container: " << c->getName());
        editor->removeChildComponent(c);
        editor->editors.removeAllInstancesOf(c);
        delete c;
    }

    return editor;

}
