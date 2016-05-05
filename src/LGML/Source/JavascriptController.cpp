/*
  ==============================================================================

    JavaScriptController.cpp
    Created: 5 May 2016 10:15:48am
    Author:  Martin Hermant

  ==============================================================================
*/

#include "JavaScriptController.h"
#include "NodeManager.h"

JavascriptController::JavascriptController(){
    nameParam->setValue( "JSController");
    jsEnv = JavascriptEnvironment::getInstance();
    jsEnv->linkToControllableContainer("node",NodeManager::getInstance());
    jsEnv->loadFile("/Users/Tintamar/Desktop/tst.js");
}

void JavascriptController::callForMessage(const OSCMessage & msg){

    if(nonValidMessages.contains(msg.getAddressPattern().toString()))return;
    
    String functionName = getJavaScriptFunctionName(msg.getAddressPattern().toString());
    JavascriptEnvironment::OwnedJsArgs args(jsEnv->localEnvironment);
    for(auto & m:msg){
        if(m.isFloat32()){
            args.addArg(m.getFloat32());
        }
        if(m.isInt32()){
            args.addArg(m.getInt32());
        }
        if(m.isString()){
            args.addArg(m.getString());
        }
    }

    Result r(Result::ok());
    jsEnv->callFunction(functionName, args.getNativeArgs(),&r);
    if(r.failed()){
        nonValidMessages.add(msg.getAddressPattern().toString());
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
    return "on"+n;

}

var JavascriptController::sendOSCFromJS(const JavascriptEnvironment::NativeFunctionArgs& a){

}

DynamicObject * JavascriptController::createOSCJsObject(){
    DynamicObject * d = new DynamicObject();
    

};