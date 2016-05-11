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

JsEnvironment::~JsEnvironment(){
    for(auto & c:listenedParameters){
        if(c.get()) c->removeParameterListener(this);
    }
    for(auto & c:listenedTriggers){
        if(c.get()) c->removeTriggerListener(this);
    }
}

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
        checkUserControllableEventFunction();
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

var JsEnvironment::callFunction (const Identifier& function, const var & arg, Result* result){
    // force Native function to explore first level global scope by setting Nargs::thisObject to undefined
    juce::var::NativeFunctionArgs Nargs(var::undefined(),&arg,1);
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


void JsEnvironment::setNamespaceName(const String & s){

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

void JsEnvironment::checkUserControllableEventFunction(){
    for(auto & c:listenedParameters){
        if(c.get()) c->removeParameterListener(this);
    }
    listenedParameters.clear();
    for(auto & c:listenedTriggers){
        if(c.get()) c->removeTriggerListener(this);
    }
    listenedTriggers.clear();

    StringArray userDefinedFunction;
    NamedValueSet root = getRootObjectProperties();
    Array<StringArray> functionNamespaces;
    for(int i = 0 ; i < root.size() ; i++ ){
        StringArray arr = splitFunctionName(root.getName(i));
        if(arr.size()>1 && arr[0] == "on"){
            arr.remove(0);
            functionNamespaces.add(arr);
        }
    }
    static const Array<ControllableContainer*> candidates={
        (ControllableContainer*)NodeManager::getInstance(),
        (ControllableContainer*)TimeManager::getInstance()
    };


    for(auto & candidate:candidates){
        Array<StringArray> concernedMethods ;
        for(auto & m:functionNamespaces){
            if(candidate->shortName == m.getReference(0)){
                StringArray localName = m;
                localName.remove(0);
                Controllable * c = candidate->getControllableForAddress(localName);
                if(Parameter * p = dynamic_cast<Parameter*>(c))
                    listenedParameters.addIfNotAlreadyThere(p);
                else if(Trigger *t = dynamic_cast<Trigger*>(c))
                    listenedTriggers.addIfNotAlreadyThere(t);
            }
        }
    }
    for(auto & c:listenedParameters){
        c->addParameterListener(this);
    }
    for(auto & t:listenedTriggers){
        t->addTriggerListener(this);
    }



}

void JsEnvironment::parameterValueChanged(Parameter * p) {
    Identifier functionName = "on_"+getJsFunctionNameFromAddress(p->getControlAddress());
    Result r(Result::ok());
    callFunction(functionName, p->value, &r);
};
void JsEnvironment::triggerTriggered(Trigger *p){
    Identifier functionName = "on_"+getJsFunctionNameFromAddress(p->getControlAddress());
    Result r(Result::ok());
    callFunction(functionName, var::undefined(), &r);

}
