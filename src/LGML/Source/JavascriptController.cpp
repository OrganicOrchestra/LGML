/*
 ==============================================================================

 JavaScriptController.cpp
 Created: 5 May 2016 10:15:48am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "JavaScriptController.h"
#include "NodeManager.h"
#include "DebugHelpers.h"
#include "JsEnvironment.h"

JavascriptController::JavascriptController():JsEnvironment("OSC.JSController"){
    nameParam->setValue("JSController");

    buildLocalEnv();

    jsPath = addStringParameter("js File Path", "path from where to load JS", "");
    jsPath->isControllableExposed = false;


}
JavascriptController::~JavascriptController(){


}


void JavascriptController::buildLocalEnv(){
    ScopedPointer<DynamicObject>  oscObj = createOSCJsObject();

	for (auto &v: variables)
	{
		oscObj->setProperty(v->parameter->shortName, v->parameter->createDynamicObject());
	}
    setLocalNamespace(*oscObj);


}

Result JavascriptController::callForMessage(const OSCMessage & msg){

    String functionName = "onCtl_"+getJsFunctionNameFromAddress(msg.getAddressPattern().toString());
    // here we choose to pass each argument as an element of a var Array value in function call i.e onMessage(ArrayList)
    var argArray;
    for(auto & m:msg){
        if(m.isFloat32()){ argArray.append(m.getFloat32());}
        if(m.isInt32()){ argArray.append(m.getInt32());}
        if(m.isString()){ argArray.append(m.getString());}
    }
    Result r(Result::ok());
    var varRes = callFunction(functionName, argArray,false , &r);
    callonAnyMsg(msg);
    return r;
}

void JavascriptController::callonAnyMsg(const OSCMessage & msg){
    var address = msg.getAddressPattern().toString();

    var args;
    for(auto & m:msg){
        if(m.isFloat32()){args.append(m.getFloat32());}
        if(m.isInt32()){args.append(m.getInt32());}
        if(m.isString()){args.append(m.getString());}
    }
    Array<var> argList = {address,args};
    static const Identifier onCtlAnyMsgIdentifier("onCtl_AnyMsg");
    var varRes = callFunctionFromIdentifier(onCtlAnyMsgIdentifier, argList);

}

Result JavascriptController::processMessageInternal(const OSCMessage &m){

	Result r1  = OSCDirectController::processMessageInternal(m);

    Result r2(Result::fail("no valid js file"));
    if(hasValidJsFile())
        r2 = callForMessage(m);
    if(!r1 && !r2){
//        NLOG("OSCController",r1.getErrorMessage());
//        NLOG("Javascript",r2.getErrorMessage());
        return Result::fail("failed");
    }

    return Result::ok();




}

inline bool appendVarToMsg(OSCMessage & msg,var & v){
    if(v.isString())msg.addString(v);
    else if(v.isDouble())msg.addFloat32((float)v);
    else if(v.isInt()) msg.addInt32(v);
    else if (v.isInt64()) msg.addInt32((int)v);// DBG("JavascriptOSC can't send int 64");
    else return false;
    
    return true;
}


var JavascriptController::sendOSCFromJS(const juce::var::NativeFunctionArgs& a){

	if(a.numArguments<2 )return var::undefined();

	if( !a.arguments[0].isString() ){
        LOG("jsOSC send first argument must be an address string");
        return var::undefined();
    }

    String address = a.arguments[0];
    if(!address.startsWithChar('/') ){
        LOG("address should start with / ");
        return var::undefined();
    }

    JavascriptController * c = getObjectPtrFromJS<JavascriptController>(a);
    OSCMessage msg(address);
    for(int i = 1 ; i < a.numArguments ; i++){
		var v = a.arguments[i];
        
        if(appendVarToMsg(msg, v)){}

        else if(v.isArray()){
            Array<var> * arr = v.getArray();
            for (int i = 0 ; i < arr->size() ; i++){
                appendVarToMsg(msg, arr->getReference(i));
            }
        }
    }

    c->sendOSC(msg);
    return var();

}

DynamicObject * JavascriptController::createOSCJsObject(){
    DynamicObject * d = new DynamicObject();
    d->setProperty(jsPtrIdentifier, (int64)this);
    static const Identifier jsSendIdentifier("send");
    d->setMethod(jsSendIdentifier, sendOSCFromJS);
    return d;

};

void JavascriptController::onContainerParameterChanged(Parameter * p) {
    OSCDirectController::onContainerParameterChanged(p);
    if(p==nameParam){
        setNamespaceName("OSC."+nameParam->stringValue());
    }
    else if (p==jsPath){
        loadFile(jsPath->stringValue());
    }
};

void JavascriptController::onContainerTriggerTriggered(Trigger * t) {

  if(t==sendAllParameters){
    sendAllParametersToJS();

  }
  OSCDirectController::onContainerTriggerTriggered(t);

};


void JavascriptController::newJsFileLoaded(){
    jsPath->setValue(getCurrentFilePath(),true);
}


