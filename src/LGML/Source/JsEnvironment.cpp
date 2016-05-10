/*
 ==============================================================================

 JsEnvironnement.cpp
 Created: 5 May 2016 9:03:35am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "JsEnvironment.h"
#include "TimeManager.h"
#include "NodeManager.h"
#include "DebugHelpers.h"

JsEnvironment::JsEnvironment(const String & ns):localNamespace(ns),_hasValidJsFile(false){
    localEnvironment = new DynamicObject();
    jsEngine.registerNativeObject(jsLocalIdentifier, getLocalEnv());
    jsEngine.registerNativeObject(jsGlobalIdentifier, getGlobalEnv());
    addToNamespace(localNamespace,getLocalEnv(),getGlobalEnv());
}

JsEnvironment::~JsEnvironment(){}

void JsEnvironment::clearNamespace(){
    while(getLocalEnv()->getProperties().size()>0){getLocalEnv()->removeProperty(getLocalEnv()->getProperties().getName(0));}
    // prune to get only core Methods and classes
    NamedValueSet root = jsEngine.getRootObjectProperties();
    for(int i = 0 ; i < root.size() ; i++){
        if(!root.getVarPointerAt(i)->isMethod()){
            Identifier id = root.getName(i);
            if(!coreJsClasses.contains(id)){
                jsEngine.registerNativeObject(id, nullptr);
            }

        }
    }
}

void    JsEnvironment::removeNamespace(const String & jsNamespace){
    removeNamespaceFromObject(jsNamespace,getLocalEnv());
}


String JsEnvironment::getParentName(){
    int idx = localNamespace.indexOfChar('.');
    return localNamespace.substring(0, idx);
}

void JsEnvironment::loadFile(const File &f){
    if(f.existsAsFile() && f.getFileExtension() == ".js"){
        internalLoadFile(f);
    }
}

void JsEnvironment::reloadFile(){
    if(!currentFile.existsAsFile())return;
    internalLoadFile(currentFile);
}

void JsEnvironment::showFile(){
    if(!currentFile.existsAsFile())return;
    currentFile.startAsProcess();
}


void JsEnvironment::internalLoadFile(const File &f ){
    StringArray destLines;
    f.readLines(destLines);
    String jsString = destLines.joinIntoString("\n");
    currentFile = f;
    // rebuild to clean namespace
    clearNamespace();
    buildLocalEnv();
    Result r=jsEngine.execute(jsString);
    if(r.failed()){
        _hasValidJsFile = false;
        LOG(r.getErrorMessage());
    }
    else{
        _hasValidJsFile = true;
        lastFileModTime = currentFile.getLastModificationTime();
        LOG("script Loaded successfully : "+f.getFullPathName());
        newJsFileLoaded();
    }

    jsListeners.call(&Listener::newJsFileLoaded,(bool)r);

}

var JsEnvironment::callFunction (const Identifier& function, const Array<var>& args, Result* result){

        // force Native function to explore first level global scope by setting Nargs::thisObject to undefined
        juce::var::NativeFunctionArgs Nargs(var::undefined(),&args.getReference(0),args.size());
        return jsEngine.callFunction(function,Nargs,result);


}

const NamedValueSet & JsEnvironment::getRootObjectProperties()  {return jsEngine.getRootObjectProperties();}
void JsEnvironment::addToLocalNamespace(const String & elem,DynamicObject *target)  {addToNamespace(elem, target,getLocalEnv());}


void JsEnvironment::setLocalNamespace(DynamicObject & target){
    clearNamespace();
    for(int i = 0 ;i < target.getProperties().size() ; i++){
        Identifier n = target.getProperties().getName(i);
        getLocalEnv()->setProperty(n, target.getProperty(n));
    }
}


void JsEnvironment::setNameSpaceName(const String & s){

    DynamicObject * d = getNamespaceFromObject(getParentName(),getGlobalEnv());
    jassert(d!=nullptr);
    d->setProperty(getModuleName(), var::undefined());
    localNamespace = s;
    d->setProperty(getModuleName(), getLocalEnv());

}

void JsEnvironment::setAutoWatch(bool s){
    if(s){
        startTimer(500);
    }
    else{
        stopTimer();
    }
}

void JsEnvironment::timerCallback(){
    Time newTime = currentFile.getLastModificationTime();
    if(newTime!=lastFileModTime){
        loadFile(currentFile);
        lastFileModTime = newTime;
    }
}
String JsEnvironment::printAllNamespace()   {return namespaceToString(jsEngine.getRootObjectProperties(),0,false,false);}


String JsEnvironment::getModuleName(){
    int idx = localNamespace.indexOfChar('.');
    if(idx==-1){return  localNamespace;}
    else{return localNamespace.substring(idx+1);}
}