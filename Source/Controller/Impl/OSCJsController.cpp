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


#include "OSCJsController.h"
#include "../../Node/Manager/NodeManager.h"
#include "../../Utils/DebugHelpers.h"
#include "../../Scripting/Js/JsEnvironment.h"
#include "../../Scripting/Js/JsHelpers.h"

#include "../ControllerFactory.h"
REGISTER_CONTROLLER_TYPE (OSCJsController);

OSCJsController::OSCJsController (StringRef name) :
    OSCDirectController (name),
    JsEnvironment ("controllers.OSCController", this)
{
    setNamespaceName ("controllers." + shortName);

    buildLocalEnv();

    jsPath = addNewParameter<StringParameter> ("js File Path", "path from where to load JS", "");
    jsPath->isControllableExposed = false;


}
OSCJsController::~OSCJsController()
{


}


void OSCJsController::buildLocalEnv()
{
    ScopedPointer<DynamicObject>  oscObj = createOSCJsObject();

    setLocalNamespace (*oscObj);


}

Result OSCJsController::callForMessage (const OSCMessage& msg)
{

    String functionName = "onCtl_" + getJsFunctionNameFromAddress (msg.getAddressPattern().toString());
    // here we choose to pass each argument as an element of a var Array value in function call i.e onCtl_XXX(ArrayList)
    var argArray;

    for (auto& m : msg)
    {
        if (m.isFloat32()) { argArray.append (m.getFloat32());}

        if (m.isInt32()) { argArray.append (m.getInt32());}

        if (m.isString()) { argArray.append (m.getString());}
    }

    Result r (Result::ok());
    var varRes = callFunction (functionName, argArray, false, &r);
    callonAnyMsg (msg);
    return r;
}


var OSCJsController::OSCArgumentToVar (OSCArgument& a)
{
    if (a.isFloat32()) { return (a.getFloat32());}

    if (a.isInt32()) { return  (a.getInt32());}

    if (a.isString()) { return (a.getString());}

    return var::undefined();
}

void OSCJsController::callonAnyMsg (const OSCMessage& msg)
{
    var address = msg.getAddressPattern().toString();

    var args;

    for (auto& m : msg)
    {
        args.append (OSCArgumentToVar (m));

    }

    Array<var> argList = {address, args};
    static const Identifier onCtlAnyMsgIdentifier ("onCtl_AnyMsg");
    var varRes = callFunctionFromIdentifier (onCtlAnyMsgIdentifier, argList);

}

Result OSCJsController::processMessageInternal (const OSCMessage& m)
{

    Result r1  = OSCDirectController::processMessageInternal (m);

    for (auto& l : jsOSCListeners) {l->processMessage (m);}

    Result r2 (Result::fail ("no valid js file"));

    if (hasValidJsFile())
        r2 = callForMessage (m);

    if (!r1 && !r2)
    {

        if(r1)
                NLOG("OSCController",r1.getErrorMessage());
        if(r2)
                NLOG("Javascript",r2.getErrorMessage());
        return Result::fail ("failed");
    }

    return Result::ok();




}

inline bool appendVarToMsg (OSCMessage& msg, var& v)
{
    if (v.isString())msg.addString (v);
    else if (v.isDouble())msg.addFloat32 ((float)v);
    else if (v.isInt()) msg.addInt32 (v);
    else if (v.isInt64()) msg.addInt32 ((int)v); // DBG("JavascriptOSC can't send int 64");
    else return false;

    return true;
}


var OSCJsController::sendOSCFromJS (const juce::var::NativeFunctionArgs& a)
{

    if (a.numArguments < 2 )return var::undefined();

    if ( !a.arguments[0].isString() )
    {
        LOG ("!!jsOSC send first argument must be an address string");
        return var::undefined();
    }

    String address = a.arguments[0];

    if (!address.startsWithChar ('/') )
    {
        LOG ("!!!address should start with / ");
        return var::undefined();
    }

    OSCJsController* c = getObjectPtrFromJS<OSCJsController> (a);
    OSCMessage msg (address);

    for (int i = 1 ; i < a.numArguments ; i++)
    {
        var v = a.arguments[i];

        if (appendVarToMsg (msg, v)) {}

        else if (v.isArray())
        {
            Array<var>* arr = v.getArray();

            for (int j = 0 ; j < arr->size() ; j++)
            {
                appendVarToMsg (msg, arr->getReference (j));
            }
        }
    }

    c->sendOSC (msg);
    return var();

}

DynamicObject*   OSCJsController::createOSCJsObject()
{
    DynamicObject* d = new DynamicObject();
    d->setProperty (jsPtrIdentifier, (int64)this);
    static const Identifier jsSendIdentifier ("send");
    d->setMethod (jsSendIdentifier, sendOSCFromJS);
    static Identifier createOSCJsListenerId ("createOSCListener");
    d->setMethod (createOSCJsListenerId, createJsOSCListener);
    return d;

};

void OSCJsController::onContainerParameterChanged (Parameter* p)
{
    OSCDirectController::onContainerParameterChanged (p);

    if (p == nameParam)
    {
        setNamespaceName ("controllers." + shortName);
    }
    else if (p == jsPath)
    {
        loadFile (jsPath->stringValue());
    }
    else if (p == enabledParam)
    {
        setScriptEnabled (enabledParam->boolValue());
    }
};

void OSCJsController::onContainerTriggerTriggered (Trigger* t)
{

    if (t == sendAllParameters)
    {
        sendAllParametersToJS();

    }

    OSCDirectController::onContainerTriggerTriggered (t);

};


void OSCJsController::newJsFileLoaded()
{

    jsPath->setValue (getCurrentFilePath(), true);
}


void OSCJsController::clearNamespace()
{
    JsEnvironment::clearNamespace();
    {
        const ScopedLock lk (jsOSCListeners.getLock());
        jsOSCListeners.clear();
    }


};



//////////////////////
// JSOSCListener


Identifier JsOSCListener::oscReceivedCallbackId ("onOSC");



var OSCJsController::createJsOSCListener (const var::NativeFunctionArgs& a)
{

    if (a.numArguments < 1) { return var::undefined();}

    OSCAddressPattern oscPattern ( a.arguments[0].toString());

    OSCJsController* originEnv = getObjectPtrFromJS<OSCJsController> (a);

    if (originEnv)
    {
        JsOSCListener* ob = new JsOSCListener (originEnv, oscPattern);
        originEnv->jsOSCListeners.add (ob);
        return ob->object;
    }


    return var::undefined();
}
inline void JsOSCListener::processMessage (const OSCMessage& msg)
{
    if (addressPattern.matches ( msg.getAddressPattern().toString()))
    {
        StringArray adList;
        adList.addTokens (msg.getAddressPattern().toString(), "/", "");

        if (adList.size())adList.remove (0);

        var jsArgs[2];
        var* addressList = &jsArgs[0];

        for (auto& a : adList) { addressList->append (a); };

        var* argsList = &jsArgs[1];

        if (msg.size() == 1) { *argsList = OSCJsController::OSCArgumentToVar (msg[0]); }
        else if (msg.size() > 1)
        {
            for (auto& m : msg) { argsList->append (OSCJsController::OSCArgumentToVar (m)); }
        }

        jsEnv->callFunctionFromIdentifier (oscReceivedCallbackId, var::NativeFunctionArgs (object, jsArgs, 2), true);
    }
}
