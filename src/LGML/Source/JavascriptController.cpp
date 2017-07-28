/*
 ==============================================================================

 JavascriptController.cpp
 Created: 5 May 2016 10:15:48am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "JavascriptController.h"
#include "NodeManager.h"
#include "DebugHelpers.h"
#include "JsEnvironment.h"
#include "JsHelpers.h"

JavascriptController::JavascriptController() :
	OSCDirectController("OSC"),
	JsEnvironment("controller.OSCController",this)
{
    setNamespaceName("controller."+shortName);

    buildLocalEnv();

    jsPath = addNewParameter<StringParameter>("js File Path", "path from where to load JS", "");
    jsPath->isControllableExposed = false;


}
JavascriptController::~JavascriptController(){


}


void JavascriptController::buildLocalEnv(){
    ScopedPointer<DynamicObject>  oscObj = createOSCJsObject();

    setLocalNamespace(*oscObj);


}

Result JavascriptController::callForMessage(const OSCMessage & msg){

    String functionName = "onCtl_"+getJsFunctionNameFromAddress(msg.getAddressPattern().toString());
    // here we choose to pass each argument as an element of a var Array value in function call i.e onCtl_XXX(ArrayList)
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


var JavascriptController::OSCArgumentToVar(OSCArgument & a){
  if(a.isFloat32()){ return(a.getFloat32());}
  if(a.isInt32()){ return  (a.getInt32());}
  if(a.isString()){ return (a.getString());}
  return var::undefined();
}

void JavascriptController::callonAnyMsg(const OSCMessage & msg){
    var address = msg.getAddressPattern().toString();

    var args;
    for(auto & m:msg){
        args.append(OSCArgumentToVar(m));

    }
    Array<var> argList = {address,args};
    static const Identifier onCtlAnyMsgIdentifier("onCtl_AnyMsg");
    var varRes = callFunctionFromIdentifier(onCtlAnyMsgIdentifier, argList);

}

Result JavascriptController::processMessageInternal(const OSCMessage &m){

	Result r1  = OSCDirectController::processMessageInternal(m);
  for(auto & l:jsOSCListeners){l->processMessage(m);}
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
            for (int j = 0 ; j < arr->size() ; j++){
                appendVarToMsg(msg, arr->getReference(j));
            }
        }
    }

    c->sendOSC(msg);
    return var();

}

DynamicObject *  JavascriptController::createOSCJsObject(){
    DynamicObject * d = new DynamicObject();
    d->setProperty(jsPtrIdentifier, (int64)this);
    static const Identifier jsSendIdentifier("send");
    d->setMethod(jsSendIdentifier, sendOSCFromJS);
    static Identifier createOSCJsListenerId("createOSCListener");
    d->setMethod(createOSCJsListenerId, createJsOSCListener);
    return d;

};

void JavascriptController::onContainerParameterChanged(Parameter * p) {
    OSCDirectController::onContainerParameterChanged(p);
    if(p==nameParam){
        setNamespaceName("controller."+shortName);
    }
    else if (p==jsPath){
        loadFile(jsPath->stringValue());
    }
    else if(p==enabledParam){
     setEnabled(enabledParam->boolValue());
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


void JavascriptController::clearNamespace(){
  JsEnvironment::clearNamespace();
  {
    const ScopedLock lk(jsOSCListeners.getLock());
    jsOSCListeners.clear();
  }


};



//////////////////////
// JSOSCListener


Identifier JsOSCListener::oscReceivedCallbackId("onOSC");



var JavascriptController::createJsOSCListener(const var::NativeFunctionArgs & a){

  if(a.numArguments<1){ return var::undefined();}

  OSCAddressPattern oscPattern( a.arguments[0].toString());

  JavascriptController * originEnv = getObjectPtrFromJS<JavascriptController>(a);
  if(originEnv){
    JsOSCListener * ob = new JsOSCListener(originEnv,oscPattern);
    originEnv->jsOSCListeners.add(ob);
    return ob->object;
  }
  

  return var::undefined();
}
inline void JsOSCListener::processMessage(const OSCMessage & msg) {
	if (addressPattern.matches( msg.getAddressPattern().toString())) {
		StringArray adList;
		adList.addTokens(msg.getAddressPattern().toString(), "/", "");
		if (adList.size())adList.remove(0);

		var jsArgs[2];
		var * addressList = &jsArgs[0];
		for (auto &a : adList) { addressList->append(a); };
		var * argsList = &jsArgs[1];
		if (msg.size() == 1) { *argsList = JavascriptController::OSCArgumentToVar(msg[0]); } else if (msg.size()>1) {
			for (auto & m : msg) { argsList->append(JavascriptController::OSCArgumentToVar(m)); }
		}

		jsEnv->callFunctionFromIdentifier(oscReceivedCallbackId, var::NativeFunctionArgs(object, jsArgs, 2), true);
	}
}
