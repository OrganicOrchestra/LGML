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

JsEnvironment::JsEnvironment(const String & ns):localNamespace(ns),_hasValidJsFile(false){
  localEnvironment = var(new DynamicObject());
  jsEngine = new JavascriptEngine();
  jsEngine->registerNativeObject(jsLocalIdentifier, getLocalEnv());
  jsEngine->registerNativeObject(jsGlobalIdentifier, getGlobalEnv());
  addToNamespace(localNamespace,getLocalEnv(),getGlobalEnv());

  startTimer(1, 20); // 50fps on update Timer
}

JsEnvironment::~JsEnvironment(){
  stopTimer(0);
  stopTimer(1);
  for(auto & c:listenedParameters){
    if(c.get()) c->removeParameterListener(this);
  }
  for(auto & c:listenedTriggers){
    if(c.get()) c->removeTriggerListener(this);
  }
  for(auto & c:listenedContainers){
    if(c.get())c->removeControllableContainerListener(this);
  }
  localEnvironment = var::undefined();
}

void JsEnvironment::clearNamespace(){
 const  ScopedLock lk(engineLock);
 jsEngine = new JavascriptEngine();
  jsEngine->registerNativeObject(jsLocalIdentifier, getLocalEnv());
  jsEngine->registerNativeObject(jsGlobalIdentifier, getGlobalEnv());
  addToNamespace(localNamespace,getLocalEnv(),getGlobalEnv());

  while(getLocalEnv()->getProperties().size()>0){getLocalEnv()->removeProperty(getLocalEnv()->getProperties().getName(0));}
  // prune to get only core Methods and classes
  //    NamedValueSet root = jsEngine->getRootObjectProperties();
  for(int i = 0 ; i < jsEngine->getRootObjectProperties().size() ; i++){
    if(!jsEngine->getRootObjectProperties().getVarPointerAt(i)->isMethod()){
      Identifier id = jsEngine->getRootObjectProperties().getName(i);
      if(!jsCoreClasses.contains(id)){
        jsEngine->registerNativeObject(id, nullptr);
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


void JsEnvironment::loadFile(const String &path) {
  File f =//(File::createFileWithoutCheckingPath(path));
    File::getCurrentWorkingDirectory().getChildFile(path);
  loadFile(f);
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

  Result r = loadScriptContent(jsString);

  jsListeners.call(&JsEnvironment::Listener::newJsFileLoaded,(bool)r);

}

Result JsEnvironment::loadScriptContent(const String & content)
{
  // rebuild to clean namespace
  clearNamespace();
  buildLocalEnv();

  // thread safe if the  environment class is not multithreaded
  Result r = Result::fail("can't lock environment");
  {
    const ScopedLock lk(engineLock);
   r= jsEngine->execute(content);
  }


  if (r.failed()) {
    _hasValidJsFile = false;
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
    static String noFunctionLog= "no function Found";
    if(result!=nullptr)result->fail(noFunctionLog);
    if(logResult)NLOG(localNamespace,noFunctionLog);
    return var::undefined();
  }
  return callFunctionFromIdentifier(function, args,logResult,result);
}

var JsEnvironment::callFunction (const String& function, const var& args,  bool logResult ,Result * result){

  if(!functionIsDefined(function)){
    static String noFunctionLog= "no function Found";
    if(result!=nullptr)result->fail(noFunctionLog);
    if(logResult)NLOG(localNamespace,noFunctionLog);
    return var::undefined();
  }
  return callFunctionFromIdentifier(function, args,logResult,result);
}



var JsEnvironment::callFunctionFromIdentifier (const Identifier& function, const Array<var>& args,bool logResult , Result* result){
  // force Native function to explore first level global scope by setting Nargs::thisObject to undefined
  bool resOwned = false;
  if (logResult && result == nullptr) {
    result = new Result(Result::ok());
    resOwned = true;
  }

  var  v ;
  juce::var::NativeFunctionArgs Nargs(var::undefined(), (args.size()>0)?&args.getReference(0):&v, args.size());
  var res;
  {
    const ScopedLock lk(engineLock);
    res =  jsEngine->callFunction(function,Nargs,result);
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

var JsEnvironment::callFunctionFromIdentifier (const Identifier& function, const var & arg,bool logResult , Result* result){
  bool resOwned = false;
  if(logResult && result==nullptr){
    result = new Result(Result::ok());
    resOwned = true;
  }

  // force Native function to explore first level global scope by setting Nargs::thisObject to undefined
  juce::var::NativeFunctionArgs Nargs(var::undefined(),&arg,1);
  var res ;
  {
  const ScopedLock lk(engineLock);
   res = jsEngine->callFunction(function,Nargs,result);
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
  d->setProperty(getModuleName(), var::undefined());
  localNamespace = s;
  d->setProperty(getModuleName(), getLocalEnv());

}

void JsEnvironment::setAutoWatch(bool s){
  if(s){
    startTimer(0,500);
  }
  else{
    stopTimer(0);
  }
}

void JsEnvironment::timerCallback(int timerID){
  if (timerID == 0)
  {
    Time newTime = currentFile.getLastModificationTime();
    if (newTime != lastFileModTime) {
      loadFile(currentFile);
      lastFileModTime = newTime;
    }
  } else if (timerID == 1)
  {
    static const Identifier onUpdateIdentifier("update");
    callFunction("onUpdate",var(),false);
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
            localName.add(f->splitedName.getUnchecked(i));
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


void JsEnvironment::sendAllParametersToJS(){
  for(auto & t:listenedTriggers){if(t.get())triggerTriggered(t);}
  for(auto & t:listenedParameters){if(t.get())parameterValueChanged(t);}
  for(auto & t:listenedContainers){
      if(t.get()){
    Array<Controllable*> conts = t->getAllControllables();
    for(auto & c:conts){controllableFeedbackUpdate(t,c);}
      }
  }
}
