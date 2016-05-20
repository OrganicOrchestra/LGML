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
    // create an object only if not skipping , if not add to parent
    if(!container->skipControllableNameInAddress)
        d = new DynamicObject();


    for(auto &c:container->controllables){
		d->setProperty(c->shortName, c->createDynamicObject());

    }

    for(auto &c:container->controllableContainers){
        if(c->isIndexedContainer()){

            if(!d->hasProperty(jsArrayIdentifier)){
                var aVar;
                DynamicObject * childObject = createDynamicObjectFromContainer(c,d);
                //check names are aligned with order (first one)
                jassert(c->getIndexedPosition() == 0);
                aVar.append(childObject);
                d->setProperty(jsArrayIdentifier, aVar);

            }
            else{
                Array<var> * arrVar;
                arrVar = d->getProperty(jsArrayIdentifier).getArray();
                //check names are aligned with order (others)
                jassert(c->getIndexedPosition() == arrVar->size());
                DynamicObject * childObject =createDynamicObjectFromContainer(c,d);
                arrVar->add(childObject);
            }
        }
        else{
            DynamicObject * childObject = createDynamicObjectFromContainer(c,d);
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
