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
#include "JavascriptControllerUI.h"
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
    setLocalNamespace(*oscObj);
}

Result JavascriptController::callForMessage(const OSCMessage & msg){

    String functionName = getJavaScriptFunctionName(msg.getAddressPattern().toString());
    // here we choose to pass each argument as a separated value in function call i.e onMessage(arg1,arg2...)
    Array<var> argArray;
    for(auto & m:msg){
        if(m.isFloat32()){argArray.add(m.getFloat32());}
        if(m.isInt32()){argArray.add(m.getInt32());}
        if(m.isString()){argArray.add(m.getString());}
    }

    Result r(Result::ok());
    var varRes = callFunction(functionName, argArray,&r);

    if(r.failed()){
        LOG("error on function : "+ functionName);
        LOG(r.getErrorMessage());
    }
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
    Result r(Result::ok());
    Array<var> argList = {address,args};
    var varRes = callFunction("onAnyMsg", argList,&r);

}

Result JavascriptController::processMessageInternal(const OSCMessage &m){
   
	Result r1  = OSCDirectController::processMessageInternal(m);

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
    d->setProperty(ptrIdentifier, (int64)this);
    d->setMethod("send", sendOSCFromJS);
    return d;

};

void JavascriptController::onContainerParameterChanged(Parameter * p) {
    OSCDirectController::onContainerParameterChanged(p);
    if(p==nameParam){
        setNamespaceName("OSC."+nameParam->stringValue());
    }
    else if (p==jsPath){
        loadFile(File(jsPath->value));
    }
};


void JavascriptController::newJsFileLoaded(){
    jsPath->setValue(getCurrentFilePath(),true);
    
    
    
}

ControllerUI * JavascriptController::createUI()
{
	return new JavascriptControllerUI(this);
}





