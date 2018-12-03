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


#ifndef JAVASCRIPTCONTROLLER_H_INCLUDED
#define JAVASCRIPTCONTROLLER_H_INCLUDED


#include "OSCDirectController.h"
#include "../../Scripting/Js/JsEnvironment.h"


class JsOSCListener;


class OSCJsController : public OSCDirectController, public JsEnvironment
{
public:
    DECLARE_OBJ_TYPE_DEFAULTNAME (OSCJsController, "OSCJS","use any OSC App to control LGML");

    ~OSCJsController();
    Result processMessageInternal (const OSCMessage& m) override;
    Result callForMessage (const OSCMessage& msg);
    void callonAnyMsg (const OSCMessage& msg);

    static var sendOSCFromJS (const juce::var::NativeFunctionArgs& a);
    void onContainerParameterChanged ( ParameterBase* p) override;
    void onContainerTriggerTriggered (Trigger* t ) override;



    



    static var createJsOSCListener (const var::NativeFunctionArgs& a);
    OwnedArray<JsOSCListener, CriticalSection> jsOSCListeners;

    static var OSCArgumentToVar (const OSCArgument& a);


private:
    void buildLocalEnv() override;
    void clearNamespace()override;

    DynamicObject*   createOSCJsObject();


    StringParameter* jsPath;
};


// jsObject to bind functions to OSC address from within js
//
//example.js
//
// var myListener = createOSCListener("/osc/pattern/to/watch")
//
// myListener.onOSC= function(value){
// gets activated when /osc/pattern/to/watch has been recieved
// do stuff with value...
// }
class JsOSCListener
{
public :
    JsOSCListener (JsEnvironment* js, OSCAddressPattern& pattern): jsEnv (js),
        addressPattern (pattern)
    {
        buildVarObject();

    }
    static Identifier oscReceivedCallbackId;


    virtual ~JsOSCListener() {};
    void buildVarObject()
    {
        object = new DynamicObject();
        DynamicObject* dob = object.getDynamicObject();
        dob->setMethod (oscReceivedCallbackId, &JsOSCListener::dummyCallback);

    }

    // overriden in Js
#pragma warning(push)
#pragma warning(disable:4305 4800)
    static var dummyCallback (const var::NativeFunctionArgs&)
    {
        return var::undefined();
    };

#pragma warning(pop)


    void processMessage (const OSCMessage& msg);


    JsEnvironment* jsEnv;
    OSCAddressPattern  addressPattern;
    var object;
};


#endif  // JAVASCRIPTCONTROLLER_H_INCLUDED
