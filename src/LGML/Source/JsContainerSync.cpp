/*
 ==============================================================================

 JsContainerSync.cpp
 Created: 9 May 2016 6:21:50pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "JsContainerSync.h"
#include "JsHelpers.h"


JsContainerSync::~JsContainerSync(){
    for(auto & n:linkedContainerNamespaces){

        if(n->container.get()) removeAllListeners(n->container);
    }
}

void JsContainerSync::removeAllListeners(ControllableContainer * c){
    for(auto & cc:c->controllableContainers){
        cc->removeControllableContainerListener(this);
        removeAllListeners(cc);
    }
}

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
        container->addControllableContainerListener(this);
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

void JsContainerSync::updateControllableNamespace(ControllableContainer * c){
    if(c==nullptr){DBG("Empty container check");return;}
    Array<Identifier> jsNamespace;
    ControllableContainer * inspected = c;

    JsContainerNamespace * originNs=nullptr;
    while(inspected &&!originNs){
        DBG("look" << inspected->shortName << (inspected->skipControllableNameInAddress?"skip":""));
        if(!inspected->skipControllableNameInAddress){
            jsNamespace.add(inspected->shortName);
        }
        for(auto & n:linkedContainerNamespaces){
            if (inspected==n->container){originNs = n;}
        }

        inspected = inspected->parentContainer;
    }


    jsNamespace.removeLast();


    if(!originNs){
        jassertfalse;
    }

    if(jsNamespace.size()==0){
        getEnv()->setProperty(originNs->nsName,createDynamicObjectFromContainer(originNs->container, nullptr));
    }
    else{

        DynamicObject * dyn = getEnv()->getProperty(originNs->nsName).getDynamicObject();

        bool found = true;
        for(int i = jsNamespace.size()-1 ; i>=1 ; i--){
            dyn = dyn->getProperty(jsNamespace[i]).getDynamicObject();
            if(dyn== nullptr){
                found = false;
                DBG("notFound Dynamic at " +jsNamespace[i].toString());
                break;
            }
            DBG(jsNamespace[i].toString());
        }


        if(found) dyn->setProperty(jsNamespace[0], createDynamicObjectFromContainer(c, dyn));
        else DBG("proprety not found for Controllable "+c->shortName);
    }

}

void JsContainerSync::childStructureChanged(ControllableContainer * c){
    aggregChanges.addNs(getContainerNamespace(c));
    //    updateControllableNamespace(c);
    
}
void JsContainerSync::childAddressChanged(ControllableContainer * c){
    aggregChanges.addNs(getContainerNamespace(c));
    //    updateControllableNamespace(c->parentContainer);
    
}
