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
#include "ControlManager.h"
#include "DebugHelpers.h"
#include "JsHelpers.h"

#include "Engine.h"

extern bool isEngineLoadingFile();
extern Engine * getEngine();

Identifier JsEnvironment::noFunctionLogIdentifier( "no function Found");
Identifier JsEnvironment::onUpdateIdentifier("onUpdate");

JsEnvironment::JsEnvironment(const String & ns,ControllableContainer * _linkedContainer):linkedContainer(_linkedContainer),localNamespace(ns),_hasValidJsFile(false),autoWatch(false),isInSyncWithLGML(false){

    
//    jsEngine = new JavascriptEngine();
//    jsEngine->registerNativeObject(jsLocalIdentifier, getLocalEnv());
//    jsEngine->registerNativeObject(jsGlobalIdentifier, getGlobalEnv());
//
  clearNamespace();
    getEngine()->addControllableContainerListener(this);
    addToNamespace(localNamespace,getLocalEnv(),getGlobalEnv());
    onUpdateTimerInterval = 20;
    
    triesToLoad = 5;
}

JsEnvironment::~JsEnvironment(){
    if(getEngine()){
        getEngine()->removeControllableContainerListener(this);
    }
    stopTimer(0);
    stopTimer(1);
  clearListeners();

    
}

void JsEnvironment::clearNamespace(){
    const  ScopedLock lk(engineLock);
    jsEngine = new JavascriptEngine();
    getLocalEnv()->clear();
    getLocalEnv()->setMethod("createParameterListener", &JsEnvironment::createParameterListenerObject);
    jsEngine->registerNativeObject(jsLocalIdentifier, getLocalEnv());
    jsEngine->registerNativeObject(jsGlobalIdentifier, getGlobalEnv());
    
    addToNamespace(localNamespace,getLocalEnv(),getGlobalEnv());


}

void    JsEnvironment::removeNamespace(const String & jsNamespace){
    removeNamespaceFromObject(jsNamespace,getLocalEnv());
}


String JsEnvironment::getParentName(){
    int idx = localNamespace.indexOfChar('.');
    return localNamespace.substring(0, idx);
}


void JsEnvironment::loadFile(const String &path) {
    
    File f =//(File::createFileWithoutCheckingPath(path));
    File::getCurrentWorkingDirectory().getChildFile(path);
    loadFile(f);
    
}

void JsEnvironment::loadFile(const File &f){
    if(f.existsAsFile() && f.getFileExtension() == ".js"){
        currentFile = f;
        if(isEngineLoadingFile()){
            startTimer(0,500);
            return;
        }
        internalLoadFile(f);
        if(!autoWatch) stopTimer(0);
    }
}

void JsEnvironment::reloadFile(){
    if(!currentFile.existsAsFile())return;
    triesToLoad = 5;
    internalLoadFile(currentFile);
}

void JsEnvironment::showFile(){
    if(!currentFile.existsAsFile())return;
    currentFile.startAsProcess();
}


void JsEnvironment::internalLoadFile(const File &f ){
    if(triesToLoad<=0){return;}
    StringArray destLines;
    f.readLines(destLines);
    String jsString = destLines.joinIntoString("\n");
    
    
    Result r = loadScriptContent(jsString);
    
    static FunctionIdentifier onUpdateFId(onUpdateIdentifier.toString());
    if(userDefinedFunctions.contains(onUpdateFId))
    {startTimer(1, onUpdateTimerInterval);}
    else{stopTimer(1);}
    
    if(r.failed() && !isInSyncWithLGML){triesToLoad--;}
    else{isInSyncWithLGML =true;}
    jsListeners.call(&JsEnvironment::Listener::newJsFileLoaded,(bool)r);
    
}

Result JsEnvironment::loadScriptContent(const String & content)
{
    // rebuild to clean namespace
    clearNamespace();
    buildLocalEnv();
    clearListeners();
    // thread safe if the  environment class is not multithreaded
    Result r = Result::fail("can't lock environment");
    {
        const ScopedLock lk(engineLock);
        r= jsEngine->execute(content);
    }
    
    
    if (r.failed()) {
        _hasValidJsFile = false;
//        jsEngine = nullptr;
        // NLOG(localNamespace,printAllNamespace());
        NLOG(localNamespace,r.getErrorMessage());
    }
    else {
        _hasValidJsFile = true;
        lastFileModTime = currentFile.getLastModificationTime();
        updateUserDefinedFunctions();
        checkUserControllableEventFunction();
        newJsFileLoaded();
        NLOG(localNamespace, "Content loaded sucessfully");
    }
    
    jsListeners.call(&JsEnvironment::Listener::jsScriptLoaded, (bool)r);
    
    return r;
}

void JsEnvironment::clearListeners(){
  for(auto & c:listenedParameters){
    if(c.get()) c->removeParameterListener(this);
  }
  listenedParameters.clear();
  for(auto & c:listenedTriggers){
    if(c.get()) c->removeTriggerListener(this);
  }
  listenedTriggers.clear();

  for(auto & c:listenedContainers){
    if(c.get())c->removeControllableContainerListener(this);
  }
  listenedContainers.clear();

  {
    const ScopedLock lk(engineLock);
    parameterListenerObjects.clear();
  }
}

bool JsEnvironment::functionIsDefined(const juce::String & s){
    bool found = false;
    for(auto & f:userDefinedFunctions){
        if(f.compare(s)){
            found = true;
            break;
        }
    }
    return found;
}



var JsEnvironment::callFunction (const String& function, const Array<var>& args, bool logResult,Result * result){
    
    if(!functionIsDefined(function)){
        
        if(result!=nullptr)result->fail(noFunctionLogIdentifier.toString());
        if(logResult)NLOG(localNamespace,noFunctionLogIdentifier.toString());
        return var::undefined();
    }
    return callFunctionFromIdentifier(function, args,logResult,result);
}

var JsEnvironment::callFunction (const String& function, const var& args,  bool logResult ,Result * result){
    
    if(!functionIsDefined(function)){
        if(result!=nullptr)result->fail(noFunctionLogIdentifier.toString());
        if(logResult)NLOG(localNamespace,noFunctionLogIdentifier.toString());
        return var::undefined();
    }
    return callFunctionFromIdentifier(function, args,logResult,result);
}



var JsEnvironment::callFunctionFromIdentifier (const Identifier& function, const Array<var>& args,bool logResult , Result* result){
    // force Native function to explore first level global scope by setting Nargs::thisObject to undefined
  var v;
    juce::var::NativeFunctionArgs Nargs(var::undefined(), (args.size()>0)?&args.getReference(0):&v, args.size());

     return callFunctionFromIdentifier(function,Nargs,logResult,result);
}

var JsEnvironment::callFunctionFromIdentifier (const Identifier& function, const var & arg,bool logResult , Result* result){

    
    // force Native function to explore first level global scope by setting Nargs::thisObject to undefined
    juce::var::NativeFunctionArgs Nargs(var::undefined(),&arg,1);
    return callFunctionFromIdentifier(function,Nargs,logResult,result);
}


var JsEnvironment::callFunctionFromIdentifier (const Identifier& function, const var::NativeFunctionArgs &Nargs,bool logResult , Result* result)
{
  if(!hasValidJsFile())return var::undefined;
  bool resOwned = false;
  if(logResult && result==nullptr){
    result = new Result(Result::ok());
    resOwned = true;
  }
  var res ;
  {
    const ScopedLock lk(engineLock);
    if(!JsGlobalEnvironment::getInstance()->isDirty()){
      res = jsEngine->callFunction(function,Nargs,result);
    }
    else{
      DBG("JS avoiding to call function while global environment is dirty");
    }
  }
  if(logResult && result->failed()){
    NLOG(localNamespace,result->getErrorMessage());
  }
  if(resOwned){
    delete result;
    result=nullptr;
  }
  return res;
}



const NamedValueSet & JsEnvironment::getRootObjectProperties()  {const ScopedLock lk(engineLock);return jsEngine->getRootObjectProperties();}
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
//    d->removeProperty(getModuleName());
    d->setProperty(getModuleName(), var::undefined());
    localNamespace = s;
    d->setProperty(getModuleName(), getLocalEnv());
    
}

void JsEnvironment::setAutoWatch(bool s){
    triesToLoad = 5;
    if(s){
        startTimer(0,500);
    }
    else{
        stopTimer(0);
    }
    
    autoWatch = s;
}

void JsEnvironment::timerCallback(int timerID){
    if (timerID == 0)
    {
        if(isEngineLoadingFile())return;
        Time newTime = currentFile.getLastModificationTime();
        if (newTime != lastFileModTime || !isInSyncWithLGML) {
            
            isLoadingFile = true;
            loadFile(currentFile);
            isLoadingFile = false;
            lastFileModTime = newTime;
        }
    } else if (timerID == 1)
    {
        
        callFunction("onUpdate",var(),true);
    }
}
String JsEnvironment::printAllNamespace()   {
    const ScopedLock lk(engineLock);
    return namespaceToString(jsEngine->getRootObjectProperties(),0,true,false);
}


String JsEnvironment::getModuleName(){
    int idx = localNamespace.indexOfChar('.');
    if(idx==-1){return  localNamespace;}
    else{return localNamespace.substring(idx+1);}
}



void JsEnvironment::checkUserControllableEventFunction(){

    
    NamedValueSet root = getRootObjectProperties();
    Array<FunctionIdentifier*> functionNamespaces;
    
    for(auto & f:userDefinedFunctions){
        if(f.splitedName[0] == "on")functionNamespaces.add(&f);
        
    }
    static const Array<ControllableContainer*> candidates={
        (ControllableContainer*)NodeManager::getInstance(),
        (ControllableContainer*)TimeManager::getInstance(),
        (ControllableContainer *)ControllerManager::getInstance()
    };
    
    
    
    for(auto & f:functionNamespaces){
        bool found = false;
        if(f->splitedName.size()>2){
            for(auto & candidate:candidates){
                if( (candidate->shortName == f->splitedName[1]) ){
                    StringArray localName;
                    // remove on and candidate Name
                    for(int i  = 2 ; i < f->splitedName.size() ; i++){
                        localName.add(f->splitedName.strings.getUnchecked(i));
                    }
                    Controllable * c = candidate->getControllableForAddress(localName);
                    if(Parameter * p = dynamic_cast<Parameter*>(c)){
                        listenedParameters.addIfNotAlreadyThere(p);
                        found = true;
                        break;
                    }
                    else if(Trigger *t = dynamic_cast<Trigger*>(c)){
                        listenedTriggers.addIfNotAlreadyThere(t);
                        found = true;
                        break;
                    }
                    else if(ControllableContainer * cont = candidate->getControllableContainerForAddress(localName)){
                        listenedContainers.addIfNotAlreadyThere(cont);
                        found = true;
                        break;
                    }
                }
                
            }
            
        }
        if(!found){
            String fName;
            for(auto & n:f->splitedName){
                fName+=n+"_";
            }
            fName = fName.substring(0, fName.length()-1);
            NLOG(localNamespace,"not found controllable/Container for function : "+fName);
        }
        
    }
    for(auto & c:listenedParameters){
        c->addParameterListener(this);
    }
    for(auto & t:listenedTriggers){
        t->addTriggerListener(this);
    }
    for(auto & cont:listenedContainers){
        cont->addControllableContainerListener(this);
    }
    
    
}

void JsEnvironment::updateUserDefinedFunctions(){
    userDefinedFunctions.clear();
    NamedValueSet root = getRootObjectProperties();
    for(int i = 0 ; i < root.size() ; i++ ){
        userDefinedFunctions.add(FunctionIdentifier(root.getName(i).toString()));
    }
    
}

void JsEnvironment::parameterValueChanged(Parameter * p) {
    if(p)callFunction("on_"+getJsFunctionNameFromAddress(p->getControlAddress()), p->value,false);
    else{jassertfalse;}
    
};
void JsEnvironment::triggerTriggered(Trigger *p){
    callFunction("on_"+getJsFunctionNameFromAddress(p->getControlAddress()), var::undefined(),false);
    
}

void JsEnvironment::controllableFeedbackUpdate(ControllableContainer *originContainer,Controllable *c){
    // avoid root callback (only used to reload if
    if(originContainer == getEngine())return;
    var v = var::undefined();
    if(Parameter * p = dynamic_cast<Parameter*>(c))
        v=p->value;
    
    String address = c->getControlAddress(originContainer);
    StringArray sArr ;sArr.addTokens(address, "/","");
    jassert(sArr.size()>0);
    sArr.remove(0);
    Array<var> add;
    for(auto & s:sArr){add.add(s);}
    Array<var> argList = {var(add),v};
    callFunction("on_"+getJsFunctionNameFromAddress(originContainer->getControlAddress()), argList,false);
}


void JsEnvironment::childStructureChanged(ControllableContainer * originContainer,ControllableContainer * notifier) {
    // rebuild files that are not loaded properly if LGML JsEnvironment is not fully built at the time of their firstExecution
    if(!_hasValidJsFile && !isLoadingFile && originContainer == getEngine() && (linkedContainer.get() && !linkedContainer->containsContainer(notifier))){
        isInSyncWithLGML = false;
        startTimer(0, 500);
    };
    
};

void JsEnvironment::sendAllParametersToJS(){
    for(auto & t:listenedTriggers){if(t.get())triggerTriggered(t);}
    for(auto & t:listenedParameters){if(t.get())parameterValueChanged(t);}
    for(auto & t:listenedContainers){
        if(t.get()){
            Array<WeakReference<Controllable>> conts = t->getAllControllables();
            for(auto & c:conts){controllableFeedbackUpdate(t,c);}
        }
    }
}


var JsEnvironment::createParameterListenerObject(const var::NativeFunctionArgs & a){
  if(a.numArguments==0){ return var::undefined();}

  if(Controllable * p = getObjectPtrFromObject<Controllable>(a.arguments[0].getDynamicObject())){
    JsEnvironment * originEnv = dynamic_cast<JsEnvironment*>(a.thisObject.getDynamicObject());
    if(originEnv){
      JsControllableListenerObject * ob = new JsControllableListenerObject(originEnv,p);
      originEnv->parameterListenerObjects.add(ob);
      return ob->object;
    }
  }
  return var::undefined();
};


//////////////////////
// JsControllableListenerObject


Identifier JsControllableListenerObject::parameterChangedFId("parameterChanged");
Identifier JsControllableListenerObject::parameterObjectId("parameter");
