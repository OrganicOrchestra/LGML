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

JavascriptController::JavascriptController():JavascriptEnvironment("OSC.JSController"){
    nameParam->setValue("JSController");

}
JavascriptController::~JavascriptController(){
    removeFromNamespace("OSC",nameParam->value);

}

void JavascriptController::buildLocalNamespace(){
        addToLocalNamespace("", createOSCJsObject());
}

Result JavascriptController::callForMessage(const OSCMessage & msg){

    if(nonValidMessages.contains(msg.getAddressPattern().toString()))return Result::ok();
    
    String functionName = getJavaScriptFunctionName(msg.getAddressPattern().toString());
//    var jsObj = getRootObjectProperties().getVarPointer("OSC")->getProperty(jsName,"");
    var* jsRcv = getRootObjectProperties().getVarPointer(getModuleName());
    if(!jsRcv)return Result::fail("No object NS");
    var jsObj = jsRcv->getProperty(functionName, var::undefined());
    if(jsObj==var::undefined()){
        nonValidMessages.add(msg.getAddressPattern().toString());
        return Result::fail("No function");
    }
    if(!jsObj.isObject())return Result::fail("No valid function");

    JavascriptEnvironment::OwnedJsArgs args(var::undefined());

    // this way add every osc element as separated argument of a function
//    for(auto & m:msg){
//        if(m.isFloat32()){args.addArg(m.getFloat32());}
//        if(m.isInt32()){args.addArg(m.getInt32());}
//        if(m.isString()){args.addArg(m.getString());}
//    }

    // this way add every osc element in an aray for variable size osc messages
    var argArray;
    for(auto & m:msg){
        if(m.isFloat32()){argArray.append(m.getFloat32());}
        if(m.isInt32()){argArray.append(m.getInt32());}
        if(m.isString()){argArray.append(m.getString());}
    }
    args.addArg(argArray);

    Result r(Result::ok());

    var varRes = callFunction(functionName, *args.getNativeArgs(),&r);

    if(r.failed()){
        LOG("error on function : "+ functionName);
        LOG(r.getErrorMessage());
    }
    

    return r;
}

void JavascriptController::callonAnyMsg(const OSCMessage & msg){
    if(hasAnyMsgMethod){
        JavascriptEnvironment::OwnedJsArgs args(var::undefined());
        for(auto & m:msg){
            if(m.isFloat32()){args.addArg(m.getFloat32());}
            if(m.isInt32()){args.addArg(m.getInt32());}
            if(m.isString()){args.addArg(m.getString());}
        }
        Result r(Result::ok());

        var varRes = callFunction("onAnyMsg", *args.getNativeArgs(),&r);
    }
}

Result JavascriptController::processMessage(const OSCMessage &m){
    Result r1  =OSCDirectController::processMessage(m);
    Result r2(Result::fail("no valid js file"));
    if(hasValidJsFile())
        r2 = callForMessage(m);
    if(!r1 && !r2){
        NLOG("OSCController",r1.getErrorMessage());
        NLOG("Javascript",r2.getErrorMessage());
        return Result::fail("failed");
    }

    return Result::ok();




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
        LOG("jsOSC send first argument must be a string");
        return var::undefined();
    }
        String address = a.arguments[0];
    if(!address.startsWithChar('/') ){
        LOG("address should start with / ");
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
        setNameSpaceName("OSC."+nameParam->stringValue());
    }
};


void JavascriptController::newJsFileLoaded(){

    nonValidMessages.clear();
    hasAnyMsgMethod = GlobalEnvironment::getInstance()->getNamespaceObject(localNamespace+".onAnyMsg")!=nullptr;
    
    
}





