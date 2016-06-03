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
    DynamicObject*  myParent = parent;
    // create an object only if not skipping , if not add to parent
    if(!container->skipControllableNameInAddress)
        myParent = new DynamicObject();
    else{
        jassert(parent!=nullptr);

    }

    for(auto &c:container->controllables){
		myParent->setProperty(c->shortName, c->createDynamicObject());


    }

    for(auto &c:container->controllableContainers){
        if(c->isIndexedContainer()){

            if(!myParent->hasProperty(jsArrayIdentifier)){
                var aVar;
                DynamicObject * childObject = createDynamicObjectFromContainer(c,myParent);
                //check names are aligned with order (first one)
                jassert(c->getIndexedPosition() == 0);
                aVar.append(childObject);
                myParent->setProperty(jsArrayIdentifier, aVar);

            }
            else{
                Array<var> * arrVar;
                arrVar = myParent->getProperty(jsArrayIdentifier).getArray();
                //check names are aligned with order (others)
                jassert(c->getIndexedPosition() == arrVar->size());
                DynamicObject * childObject =createDynamicObjectFromContainer(c,myParent);
                arrVar->add(childObject);
            }
        }
        else{
            DynamicObject * childObject = createDynamicObjectFromContainer(c,myParent);
            if(!c->skipControllableNameInAddress && childObject!=nullptr)
                myParent->setProperty(c->shortName, var(childObject));
        }
    }

    return myParent;
}



void JsContainerSync::childStructureChanged(ControllableContainer * c){
    if(JsContainerNamespace * ns= getContainerNamespace(c)){
        getEnv()->setProperty(ns->nsName, createDynamicObjectFromContainer(ns->container, nullptr));
    }
}
void JsContainerSync::childAddressChanged(ControllableContainer * c){
    // for now we rebuild all js (calls are made from root containers like node or time
    // we could implement a nicer implementation where only changed child get's updated
    
    if(JsContainerNamespace * ns= getContainerNamespace(c)){
        getEnv()->setProperty(ns->nsName, createDynamicObjectFromContainer(ns->container, nullptr));
    }
}
