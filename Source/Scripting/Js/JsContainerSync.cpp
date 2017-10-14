/* Copyright © Organic Orchestra, 2017
 *
 * This file is part of LGML.  LGML is a software to manipulate sound in realtime
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation (version 3 of the License).
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */


#include "JsContainerSync.h"
#include "JsHelpers.h"
#include "JsEnvironment.h"


JsContainerSync::~JsContainerSync()
{
    for (auto& n : linkedContainerNamespaces)
    {

        if (auto c = n->container.get()) c->removeControllableContainerListener (this);
    }
}



void JsContainerSync::linkToControllableContainer (const String& controllableNamespace, ControllableContainer* c)
{
    c->addControllableContainerListener (this);
    auto obj = createDynamicObjectFromContainer (c, nullptr);

    if (!existInContainerNamespace (controllableNamespace))
    {
        linkedContainerNamespaces.add (new JsContainerNamespace (controllableNamespace, c, obj)) ;
    }

    getEnv()->setProperty (controllableNamespace, obj);
}



JsContainerSync::JsContainerNamespace* JsContainerSync::getContainerNamespace (ControllableContainer* c)
{
    JsContainerNamespace* result = nullptr;
    jassert (c != nullptr);

    for (auto& n : linkedContainerNamespaces)
    {
        ControllableContainer* inspected = c;

        while (inspected != nullptr)
        {
            if (n->container == inspected)
            {
                return n;
            }

            inspected = inspected->parentContainer;
        }

    }

    return result;
}


JsContainerSync::JsContainerNamespace* JsContainerSync::getContainerNamespace (const String& ns)
{
    JsContainerNamespace* result = nullptr;

    for (auto& n : linkedContainerNamespaces)
    {
        if (n->nsName == ns) return n;
    }

    return result;
}

bool JsContainerSync::existInContainerNamespace (const String& ns)
{
    return getContainerNamespace (ns) != nullptr;
}

//WIP : jsMethod for getting children controllable by name

// support OSC style string / arrays and lists
var getControllableForAddress (const var::NativeFunctionArgs& a)
{
    ControllableContainer* callerCont = getObjectPtrFromJS<ControllableContainer> (a);

    if (!callerCont)return var::undefined();

    StringArray arr;

    if (a.numArguments == 1)
    {
        if (a.arguments[0].isArray()) {for (auto e : *a.arguments[0].getArray()) {arr.add (e);};};

        if (a.arguments[0].isString())
        {
            arr.addTokens (a.arguments[0].toString(), "/", "");

            if (arr.size())arr.remove (0);
        };
    }
    else if (a.numArguments > 1)
    {
        for (int i = 0 ; i < a.numArguments; i ++ ) {arr.add (a.arguments[i].toString());}
    }

    if (arr.size())
    {
        Controllable* res = callerCont->getControllableForAddress (arr);

        if (res) {return res->createDynamicObject();}
    }

    return var::undefined();

};

DynamicObject*
JsContainerSync::createDynamicObjectFromContainer (ControllableContainer* container, DynamicObject* parent)
{
    DynamicObject*  myParent = parent;


    if (auto js = dynamic_cast<JsEnvironment*> (container))
    {
        myParent = js->localEnv;
    }
    else
    {
        myParent = new DynamicObject();
    }




    static Identifier getControllableForAddressId ("getControllableForAddress");
    myParent->setMethod (getControllableForAddressId, getControllableForAddress);
    myParent->setProperty (jsPtrIdentifier, (int64)container);

    for (auto& c : container->controllables)
    {
        myParent->setProperty (c->shortName, c->createDynamicObject());


    }

    for (auto& c : container->controllableContainers)
    {
        if (c.get())
        {


            if (c->isIndexedContainer())
            {

                if (!myParent->hasProperty (jsArrayIdentifier))
                {
                    var aVar;
                    auto childObject = createDynamicObjectFromContainer (c, myParent);
                    //check names are aligned with order (first one)
                    jassert (c->getIndexedPosition() == 0);
                    aVar.append (childObject);
                    myParent->setProperty (jsArrayIdentifier, aVar);

                }
                else
                {
                    Array<var>* arrVar;
                    arrVar = myParent->getProperty (jsArrayIdentifier).getArray();
                    //check names are aligned with order (others)
                    jassert (c->getIndexedPosition() == arrVar->size());
                    auto childObject = createDynamicObjectFromContainer (c, myParent);
                    arrVar->add (childObject);
                }
            }
            else
            {
                auto childObject = createDynamicObjectFromContainer (c, myParent);

                if (childObject != nullptr)
                    myParent->setProperty (c->shortName, childObject);
            }
        }
    }

    return myParent;
}

void JsContainerSync::updateControllableNamespace (ControllableContainer* c)
{
    if (c == nullptr) {DBG ("Empty container check"); return;}

    Array<Identifier> jsNamespace;
    ControllableContainer* inspected = c;

    JsContainerNamespace* originNs = nullptr;

    while (inspected && !originNs)
    {
        DBG ("look" << inspected->shortName );


        jsNamespace.add (inspected->shortName);


        for (auto& n : linkedContainerNamespaces)
        {
            if (inspected == n->container) {originNs = n;}
        }

        inspected = inspected->parentContainer;
    }


    jsNamespace.removeLast();


    if (!originNs)
    {
        jassertfalse;
    }

    if (jsNamespace.size() == 0)
    {
        getEnv()->setProperty (originNs->nsName, createDynamicObjectFromContainer (originNs->container, nullptr));
    }
    else
    {

        DynamicObject* dyn = getEnv()->getProperty (originNs->nsName).getDynamicObject();

        bool found = true;

        for (int i = jsNamespace.size() - 1 ; i >= 1 ; i--)
        {
            dyn = dyn->getProperty (jsNamespace[i]).getDynamicObject();

            if (dyn == nullptr)
            {
                found = false;
                DBG ("notFound Dynamic at " + jsNamespace[i].toString());
                break;
            }

            DBG (jsNamespace[i].toString());
        }


        if (found) dyn->setProperty (jsNamespace[0], createDynamicObjectFromContainer (c, dyn));
        else DBG ("proprety not found for Controllable " + c->shortName);
    }

}
bool JsContainerSync::isDirty()
{
    return aggregChanges.nsToUpdate.size() > 0;
}
void JsContainerSync::childStructureChanged (ControllableContainer* notifier, ControllableContainer* ,bool isAdded)
{
    
    aggregChanges.addNs (getContainerNamespace (notifier));
    //    updateControllableNamespace(c);
    
}
void JsContainerSync::childAddressChanged (ControllableContainer* notifier,ControllableContainer* c)
{
    aggregChanges.addNs (getContainerNamespace (c));
    //    updateControllableNamespace(c->parentContainer);
    
}
