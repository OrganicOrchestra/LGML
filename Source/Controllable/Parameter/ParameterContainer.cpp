/*
 ==============================================================================

 ParameterContainer.cpp
 Created: 5 Sep 2017 3:59:43pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "ParameterContainer.h"
#include "../Parameter/ParameterFactory.h"

//#include "../Parameter/NumericParameter.h"
//#include "../Parameter/BoolParameter.h"
//#include "../Parameter/StringParameter.h"
//#include "../Parameter/EnumParameter.h"
//#include "../Parameter/RangeParameter.h"
//#include "../Parameter/ParameterList.h"
//#include "../Parameter/Trigger.h"

//#include "../../Preset/PresetManager.h"
#include "../../Preset/Preset.h"


const Identifier ParameterContainer::uidIdentifier ("uid");

IMPL_OBJ_TYPE (ParameterContainer)

ParameterContainer::ParameterContainer (StringRef niceName):
ControllableContainer (niceName),
isHidenInEditor(false),
_canHavePresets(true),
_presetSavingIsRecursive(true)
{

    

    nameParam = addNewParameter<StringParameter> ("Name", "Set the visible name", niceName);
    nameParam->isPresettable = false;

    containSavableObjects = addNewParameter<BoolParameter>("Contains Savable Objects", "this container is allowed to add customObjects", false);
    containSavableObjects->setInternalOnlyFlags(false,true);
    containSavableObjects->isPresettable = false;
    presetable = std::make_unique<Presetable>(this);


    // init once all are created to avoid uninitialized values
    //    setNiceName(niceName);

}

ParameterContainer::~ParameterContainer()
{
    ParameterContainer::masterReference.clear();
}
const String  ParameterContainer::getNiceName()
{
    return nameParam->stringValue();
}

String ParameterContainer::setNiceName (const String& n)
{
    auto targetName = ControllableContainer::setNiceName (n);
    nameParam->setValue (targetName, targetName == getNiceName(), true);
    return targetName;
}


Array<WeakReference<ParameterBase>> ParameterContainer::getAllParameters (bool recursive, bool getNotExposed)
{
    Array<WeakReference<ParameterBase>> result;

    for (auto& c : controllables)
    {
        if (getNotExposed || c->isControllableExposed)
        {
            if ( ParameterBase* cc = ParameterBase::fromControllable (c))
            {
                result.add (cc);
            }
        }
    }

    if (recursive)
    {
        ScopedLock lk (controllableContainers.getLock());

        for (auto& cc : controllableContainers)
        {
            if (cc.get())
                result.addArray (((ParameterContainer*)cc.get())->getAllParameters (true, getNotExposed));
        }
    }

    return result;
}

int ParameterContainer::getDepthDistanceFromChildContainer(ParameterContainer * from){
    ControllableContainer * insp =from;
    int d = 0;
    while(insp){
        if(insp==this){
            return d;
        }
        insp = insp->parentContainer;
        d++;
    }
    jassertfalse;
    return -1;
}

DynamicObject* ParameterContainer::createObject(){
    return createObjectMaxDepth(-1);
}

DynamicObject* ParameterContainer::createObjectMaxDepth(int maxDepth)
{

    DynamicObject* data = new DynamicObject();
    data->setProperty (factoryTypeIdentifier, getFactoryTypeName());
    data->setProperty (uidIdentifier, uid.toString());
    {
        var paramsData (new DynamicObject);

        for (auto& c : controllables)
        {
            if (c->isUserDefined || c->isSavableAsObject)
            {

                paramsData.getDynamicObject()->setProperty (c->niceName, ParameterFactory::createTypedObjectFromInstance ( ParameterBase::fromControllable (c)));
            }
            else if (c->isSavable)
            {
                paramsData.getDynamicObject()->setProperty (c->niceName, c->getVarState());
            }
        }

        if(paramsData.getDynamicObject()->getProperties().size())
            data->setProperty (controllablesId, paramsData);

    }


    if (controllableContainers.size() && maxDepth!=0)
    {
        DynamicObject*   childData = new DynamicObject();

        for (auto controllableCont : getContainersOfType<ParameterContainer>(false))
        {
            DynamicObject * obj =controllableCont->createObject();
            if(obj && obj->getProperties().size())
                childData->setProperty (controllableCont->getNiceName(), obj);
        }
        if(childData->getProperties().size())
            data->setProperty (childContainerId, childData);
    }



    return data;
}

DynamicObject* ParameterContainer::createObjectFiltered(std::function<bool(ParameterBase*)> controllableFilter,std::function<bool(ParameterContainer*)> containerFilter,int maxDepth,bool includeUID,bool getNotExposed)
{

    DynamicObject* data = new DynamicObject();
    data->setProperty (factoryTypeIdentifier, getFactoryTypeName());
    if(includeUID)data->setProperty (uidIdentifier, uid.toString());
    {
        var paramsData (new DynamicObject);

        for (auto& c : getAllParameters(false,getNotExposed))
        {

            if(controllableFilter(c)){
                if (c->isUserDefined || c->isSavableAsObject)
                {

                    paramsData.getDynamicObject()->setProperty (c->niceName, ParameterFactory::createTypedObjectFromInstance ( ParameterBase::fromControllable (c)));
                }
                else if (c->isSavable)
                {
                    paramsData.getDynamicObject()->setProperty (c->niceName, c->getVarState());
                }
            }
        }


        data->setProperty (controllablesId, paramsData);

    }


    if (controllableContainers.size()&& maxDepth!=0)
    {
        DynamicObject*   childData = new DynamicObject();

        for (auto controllableCont : getContainersOfType<ParameterContainer>(false))
        {
            if(containerFilter(controllableCont))
                childData->setProperty (controllableCont->getNiceName(), controllableCont->createObjectFiltered(controllableFilter,containerFilter,maxDepth-1,includeUID,getNotExposed));
        }
        if(childData->getProperties().size())
            data->setProperty (childContainerId, childData);
    }
    
    
    
    return data;
}



void ParameterContainer::childControllableRemoved (ControllableContainer*, Controllable* c)
{

    if (auto p = ParameterBase::fromControllable (c))
    {
        p->removeParameterListener (this);
        p->removeAsyncParameterListener (this);
    }
}

ParameterBase*   ParameterContainer::addParameter ( ParameterBase* p,int idxToInsert,bool doListen)
{
    if(p){
        p->setParentContainer (this);
        if(idxToInsert==-1)controllables.add (p);
        else controllables.insert( idxToInsert,p);


        controllableContainerListeners.call (&ControllableContainerListener::childControllableAdded, this, p);
        notifyStructureChanged (this,true,true,false);
        if(doListen){
            p->addParameterListener (this);
            p->addAsyncParameterListener (this);
        }
        p->isUserDefined = p->isUserDefined || isUserDefined;
#if JUCE_DEBUG
//        jassert(!p->isUserDefined);
#endif
        if(p->isSavableAsObject){
            containSavableObjects->setValue(true,true);
        }
    }
    else{
        //        jassertfalse;
    }
    return p;
}





void ParameterContainer::newMessage (const ParameterBase::ParamWithValue& pv)
{
    if (!pv.isRange())
    {
        onContainerParameterChangedAsync (pv.parameter, pv.value);
    }
}

void ParameterContainer::parameterValueChanged ( ParameterBase* p, ParameterBase::Listener * notifier)
{
    if(notifier!=this){
        if (p == nameParam)
        {
            if (parentContainer)
            {
                String oN = nameParam->stringValue();
                String tN = parentContainer->getUniqueNameInContainer (oN, 0, this);

                if (tN != oN)
                {
                    nameParam->setValue (tN, false, true);
                }
            }

            setAutoShortName();
        }
        


        if (p && p->getFactoryTypeId() == Trigger::_factoryType)
        {
            onContainerTriggerTriggered ((Trigger*)p);
        }
        else
        {
            onContainerParameterChanged (p);
        }
    }

    if ( (p != nullptr && p->parentContainer == this ) ){
        if(p->isControllableExposed)
            dispatchFeedback (p,notifier);
        
    }
}

void ParameterContainer::addFeedbackListener (FeedbackListener* newListener,bool listenToDirectChild) {
    addControllableContainerListener ((ControllableContainer::Listener*)newListener);

    if(listenToDirectChild){
        directControllableContainerFBListeners.add(newListener);
    }
    else{
        controllableContainerFBListeners.add (newListener);
    }
    newListener->listenedFBContainers.addIfNotAlreadyThere(this);
}
void ParameterContainer::removeFeedbackListener (FeedbackListener* listener) {
    removeControllableContainerListener((ControllableContainer::Listener*)listener);
    controllableContainerFBListeners.remove (listener);
    directControllableContainerFBListeners.remove(listener);
    listener->listenedFBContainers.removeAllInstancesOf(this);
}



void ParameterContainer::dispatchFeedback (ParameterBase* c, ParameterBase::Listener * notifier) // from root to notifier
{
    directControllableContainerFBListeners.call (&FeedbackListener::parameterFeedbackUpdate, this, c,notifier);
    dispatchFeedbackInternal(c,notifier);
}
void ParameterContainer::dispatchFeedbackInternal(ParameterBase* c, ParameterBase::Listener * notifier){
    if (auto pc = dynamic_cast<ParameterContainer*>(parentContainer)) { pc->dispatchFeedbackInternal (c,notifier); }

    controllableContainerFBListeners.call (&FeedbackListener::parameterFeedbackUpdate, this, c,notifier);

}
void ParameterContainer::configureFromObject (DynamicObject* dyn)
{
    if (dyn)
    {

        if (dyn->hasProperty (uidIdentifier)) uid = dyn->getProperty (uidIdentifier);

        {
            DynamicObject* paramsData = dyn->getProperty (controllablesId).getDynamicObject();
            jassert (paramsData);

            if (paramsData != nullptr)
            {
                auto props = paramsData->getProperties();

                for (auto& p : props)
                {
                    Controllable* c = getControllableByName (p.name.toString()); // retro compat : try both
                    if(!c){c= getControllableByShortName (p.name.toString());}
                    if (c)
                    {
                        if (c->isSavable)
                        {
                            if ( ParameterBase* par = ParameterBase::fromControllable (c))
                            {
                                // we don't load preset when already loading a state
                                if ((par->shortName != Presetable::presetIdentifier )  || !dyn->hasProperty(childContainerId) )
                                {
                                    if(par->isSavableAsObject){
                                        if(auto d = p.value.getDynamicObject()){
                                            jassert(d->hasProperty("value"));
                                            par->setValue (d->getProperty("value"));
                                        }
                                        else{

                                            LOGE(String("parsing error for parameter 123").replace("123",par->niceName));
                                            par->setValue (p.value); // retro compat try to set value
                                        }
                                    }
                                    else{
                                        par->setValue (p.value);
                                    }
                                }
                                else{
                                    
                                    DBG("avoid loading  preset because child properties are present");
                                }
                            }
                            else
                            {
                                // we don't use custom types for now
                                jassertfalse;
                            }
                        }
                    }
                    else
                    {
                        addParameterFromVar (p.name.toString(), p.value);
                    }

                }
            }
        }

        {


            auto cD = dyn->getProperty (childContainerId).getDynamicObject();

            if (cD)
            {
                const auto ob = cD->getProperties();

                for (const auto& o : ob)
                {
                    auto cont = dynamic_cast<ParameterContainer*> (getControllableContainerByName (o.name.toString()));

                    if (cont)
                    {
                        cont->configureFromObject (o.value.getDynamicObject());
                    }
                    else if(canHaveUserDefinedContainers)
                    {
                        if( o.value.getDynamicObject()){
                            auto c = addContainerFromObject (o.name.toString(), o.value.getDynamicObject());

                            if (!c)
                            {
                                jassertfalse;
                            }
                        }
                        else
                        {
                            jassertfalse;
                        }
                    }
                    else{
                        LOGE(String("sub container 456 not allowed in 123").replace("123", getNiceName()).replace("456",o.name.toString()));
                    }
                }

            }
        }

    }
    else
    {
        jassertfalse;
    }




}


ParameterContainer*   ParameterContainer::addContainerFromObject (const String& name, DynamicObject*   data)
{
    auto res = new ParameterContainer (name);
    res->setUserDefined (true);
    addChildControllableContainer (res);
    res->configureFromObject(data);
    return res;;
};
ParameterBase* ParameterContainer::addParameterFromVar (const String& name, const var& data)
{
    // handle automagically for userdefined
    if (isUserDefined || containSavableObjects->boolValue())
    {
        ParameterBase * up {nullptr};
        if (data.isObject())
        {
            up = addParameter (ParameterFactory::createBaseFromObject ( name, data.getDynamicObject()));
        }
        else
        {
            up = addParameter (ParameterFactory::createBaseFromVar (name, data));
        }
        if(up){
            up->isUserDefined = true;
        }
        return up;
    }
    else
    {
        jassertfalse;
        return nullptr;
    }
};


void ParameterContainer::clearUserDefinedParameters(){
    for(auto p:getAllParameters()){
        if(p.get() && p->isUserDefined){
            removeControllable(p);
        }
    }
}
ParameterContainer * ParameterContainer::getForUid(const Uuid &ui){
    if(ui==uid){
        return this;
    }
    return dynamic_cast<ParameterContainer*>(
                                             findFirstControllableContainer([ui](ControllableContainer* c){return c->uid==ui;}));
}
ParameterContainer * ParameterContainer::getForUidGlobal(const Uuid &ui){
    return dynamic_cast<ParameterContainer*>(ControllableContainer::globalRoot)->getForUid(ui);

}

void ParameterContainer::setUserDefined (bool v)
{
    ControllableContainer::setUserDefined (v);
    nameParam->isEditable = v;
    
}

