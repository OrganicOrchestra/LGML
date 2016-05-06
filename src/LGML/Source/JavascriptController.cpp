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

JavascriptController::JavascriptController(){

    jsName = "JSController";

    jsEnv = JavascriptEnvironment::getInstance();
//    jsEnv->linkToControllableContainer("node",NodeManager::getInstance());
    jsEnv->addToNamespace("OSC", jsName, createOSCJsObject());
    jsEnv->loadFile("/Users/Tintamar/Desktop/tst.js");

    DBGLOG(JSON::toString (var((DynamicObject *)jsEnv->root.get())));

    nameParam->setValue( jsName);
}
JavascriptController::~JavascriptController(){
    jsEnv->removeFromNamespace("OSC",nameParam->value);

}
void JavascriptController::callForMessage(const OSCMessage & msg){

    if(nonValidMessages.contains(msg.getAddressPattern().toString()))return;
    
    String functionName = getJavaScriptFunctionName(msg.getAddressPattern().toString());
//    var jsObj = jsEnv->getRootObjectProperties().getVarPointer("OSC")->getProperty(jsName,"");
    var* jsRcv = jsEnv->getRootObjectProperties().getVarPointer("OSCRcv");
    if(!jsRcv)return;
    var jsObj = jsRcv->getProperty(functionName, var::undefined());
    if(jsObj==var::undefined()){
        nonValidMessages.add(msg.getAddressPattern().toString());
        return;
    }
    jassert(jsObj.isObject());
    JavascriptEnvironment::OwnedJsArgs args(*jsRcv);
    for(auto & m:msg){
        if(m.isFloat32()){args.addArg(m.getFloat32());}
        if(m.isInt32()){args.addArg(m.getInt32());}
        if(m.isString()){args.addArg(m.getString());}
    }

    Result r(Result::ok());

    var varRes = jsEnv->callFunction(functionName, args.getNativeArgs(),&r);

    if(r.failed()){

        DBG("============Javascript error==============");
        DBG("error on function : "+ functionName);
        DBG(r.getErrorMessage());
    }
    


}

void JavascriptController::processMessage(const OSCMessage &m){
    OSCDirectController::processMessage(m);
    callForMessage(m);
    
}

String JavascriptController::getJavaScriptFunctionName(const String & n){
    StringArray arr;
    arr.addTokens(n, "/","");
    arr.remove(0);
    String methodName ="";
    for(auto& a:arr.strings ){
        String upperCase = a.replaceSection(0, 1, a.substring(0, 1).toUpperCase());
        methodName+= upperCase;
    }
     arr.joinIntoString("");
    return "on"+methodName;
//    return "OSC."+jsName+".on"+methodName;

}

var JavascriptController::sendOSCFromJS(const JavascriptEnvironment::NativeFunctionArgs& a){
    if(a.numArguments<2 )return var::undefined();
    if( !a.arguments[0].isString() ){
        DBG("jsOSC send first argument must be a string");
        return var::undefined();
    }
        String address = a.arguments[0];
    if(!address.startsWithChar('/') ){
        DBG("address should start with / ");
        return var::undefined();
    }


    DynamicObject * d = a.thisObject.getDynamicObject();
    JavascriptController * c = dynamic_cast<JavascriptController*>((JavascriptController*)(int64)d->getProperty("_ptr"));
    OSCMessage msg(address);
    for(int i = 1 ; i < a.numArguments ; i++){
        var v = a.arguments[i];
        if(v.isString())msg.addString(v);
        else if(v.isDouble())msg.addFloat32((float)v);
        else if(v.isInt())msg.addInt32(v);
        else if (v.isInt64())DBG("JavascriptOSC can't send int 64");
    }

    c->sender.send(msg);
    return var();

}

DynamicObject * JavascriptController::createOSCJsObject(){
    DynamicObject * d = new DynamicObject();
    d->setProperty("_ptr", (int64)this);
    d->setMethod("send", sendOSCFromJS);
    return d;

};

void JavascriptController::onContainerParameterChanged(Parameter * p) {
    OSCDirectController::onContainerParameterChanged(p);
    if(p==nameParam){
        jsEnv->removeFromNamespace("OSC", jsName);
        jsName = nameParam->value;
        jsEnv->addToNamespace("OSC", jsName,createOSCJsObject());
    }
};

