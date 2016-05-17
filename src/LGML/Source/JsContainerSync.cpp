/*
 ==============================================================================

 JsContainerSync.cpp
 Created: 9 May 2016 6:21:50pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "JsContainerSync.h"
#include "JsHelpers.h"


void JsContainerSync::linkToControllableContainer(const String & controllableNamespace,ControllableContainer * c){
    c->addControllableContainerListener(this);
    DynamicObject * obj = createDynamicObjectFromContainer(c,nullptr);
    if(!existInContainerNamespace(controllableNamespace)){
        linkedContainerNamespaces.add(new JsContainerNamespace(controllableNamespace,c,obj)) ;
    }

    getEnv()->setProperty(controllableNamespace, obj);
}



JsContainerSync::JsContainerNamespace* JsContainerSync::getContainerNamespace(ControllableContainer * c){
    JsContainerNamespace* result = nullptr;
    jassert(c!=nullptr);
    for(auto & n:linkedContainerNamespaces){
        ControllableContainer * inspected = c;
        while(inspected!=nullptr){
            if(n->container == inspected){
                return n;
            }
            inspected = inspected->parentContainer;
        }

    }
    return result;
}


JsContainerSync::JsContainerNamespace* JsContainerSync::getContainerNamespace(const String & ns){
    JsContainerNamespace* result = nullptr;

    for(auto & n:linkedContainerNamespaces){
        if(n->nsName == ns) return n;
    }

    return result;
}

bool JsContainerSync::existInContainerNamespace(const String & ns){
    return getContainerNamespace(ns)!=nullptr;
}

DynamicObject* JsContainerSync::createDynamicObjectFromContainer(ControllableContainer * container,DynamicObject *parent)
{
    DynamicObject*  d = parent;
    if(!container->skipControllableNameInAddress)
        d = new DynamicObject();


    for(auto &c:container->controllables){
		d->setProperty(c->shortName, c->createDynamicObject());
    }

    for(auto &c:container->controllableContainers){
        jassert(c->shortName.isNotEmpty());
        bool isNumber = true;
        juce::String::CharPointerType cc = c->shortName.getCharPointer();
        while (!cc.isEmpty()){
            isNumber &= cc.isDigit();
            cc.getAndAdvance();
        }
        if(isNumber){

            static const Identifier ArrayIdentifier("elements");

            if(!d->hasProperty(ArrayIdentifier)){
                var aVar;
                DynamicObject * childObject = createDynamicObjectFromContainer(c,d);
                jassert(c->shortName.getIntValue() == 0);
                aVar.append(childObject);
                // skiping not handled
                jassert(!c->skipControllableNameInAddress);
                d->setProperty(ArrayIdentifier, aVar);

            }
            else{
                Array<var> * arrVar;
                arrVar = d->getProperty(ArrayIdentifier).getArray();
                jassert(c->shortName.getIntValue() == arrVar->size());
                DynamicObject * childObject =createDynamicObjectFromContainer(c,d);
                arrVar->add(childObject);
            }
        }
        else{
            DynamicObject * childObject = createDynamicObjectFromContainer(c,d);
            if(!c->skipControllableNameInAddress)
                d->setProperty(c->shortName, childObject);
        }
    }

    return d;
}



void JsContainerSync::childStructureChanged(ControllableContainer * c){
    if(JsContainerNamespace * ns= getContainerNamespace(c)){
        getEnv()->setProperty(ns->nsName, createDynamicObjectFromContainer(ns->container, nullptr));
    }
}
