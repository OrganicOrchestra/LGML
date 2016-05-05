/*
  ==============================================================================

    JavaScriptController.cpp
    Created: 5 May 2016 10:15:48am
    Author:  Martin Hermant

  ==============================================================================
*/

#include "JavaScriptController.h"

JavascriptController::JavascriptController(JavascriptEnvironment * ev):jsEnv(ev){
    jassert(ev);
    jsEnv->loadTest();
}

void JavascriptController::callForMessage(const String & msg){
    StringArray arr;
    arr.addTokens(msg, " ");
    if(arr.size()==0){
        jassertfalse;
        return;
    }

    String functionName = getJavaScriptFunctionName(arr[0]);
    arr.remove(0);
    JavascriptEnvironment::OwnedJsArgs args(jsEnv->localEnvironment);
    args.addArgs(arr);
    Result r(Result::ok());
    jsEnv->callFunction(functionName, args.getNativeArgs(),&r);
    if(r.failed()){
        DBG("============Javascript error==============");
        DBG("error on function : "+ functionName);
        DBG(r.getErrorMessage());
    }



}

String JavascriptController::getJavaScriptFunctionName(const String & n){
    return "on"+n;

}