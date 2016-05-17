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
//    NamedValueSet root = jsEngine.getRootObjectProperties();
    for(int i = 0 ; i < jsEngine.getRootObjectProperties().size() ; i++){
        if(!jsEngine.getRootObjectProperties().getVarPointerAt(i)->isMethod()){
            Identifier id = jsEngine.getRootObjectProperties().getName(i);
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
   
	Result r = loadScriptContent(jsString);

    jsListeners.call(&JsEnvironment::Listener::newJsFileLoaded,(bool)r);

}

Result JsEnvironment::loadScriptContent(const String & content)
{
	// rebuild to clean namespace
	clearNamespace();
	buildLocalEnv();
	Result r = jsEngine.execute(content);

	if (r.failed()) {
		_hasValidJsFile = false;
		NLOG("JS",r.getErrorMessage());
	}
	else {
		_hasValidJsFile = true;
		lastFileModTime = currentFile.getLastModificationTime();
		updateUserDefinedFunctions();
		checkUserControllableEventFunction();
		newJsFileLoaded();
		NLOG("JS", "Content loaded sucessfully");
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

    if(logResult)jassert( result!=nullptr);

    if(!functionIsDefined(function)){
        if(result!=nullptr)result->fail("no function Found");
        return var::undefined();
    }
    return callFunctionFromIdentifier(function, args,logResult,result);
}

var JsEnvironment::callFunction (const String& function, const var& args,  bool logResult ,Result * result){

    if(!functionIsDefined(function)){
        if(result!=nullptr)result->fail("no function Found");
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

	var * v = new var(); //@martin can this give a memory leak ? need to handle calls without arguments
	juce::var::NativeFunctionArgs Nargs(var::undefined(), (args.size()>0)?&args.getReference(0):v, args.size());

    var res =  jsEngine.callFunction(function,Nargs,result);
    if(logResult && result->failed()){
        LOG(result->getErrorMessage());
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
    var res =  jsEngine.callFunction(function,Nargs,result);
    if(logResult && result->failed()){
        LOG(result->getErrorMessage());
    }
    if(resOwned){
        delete result;
        result=nullptr;
    }
    return res;
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


    Array<FunctionIdentifier*> functionNamespaces;

    for(auto & f:userDefinedFunctions){
            if(f.splitedName[0] == "on")functionNamespaces.add(&f);

    }
    static const Array<ControllableContainer*> candidates={
        (ControllableContainer*)NodeManager::getInstance(),
        (ControllableContainer*)TimeManager::getInstance()
    };


    for(auto & candidate:candidates){
        Array<StringArray> concernedMethods ;
        for(auto & f:functionNamespaces){
            if(candidate->shortName == f->splitedName[1]){
                StringArray localName = f->splitedName;
                // remove on
                localName.remove(0);
                //remove candidate Name
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

void JsEnvironment::updateUserDefinedFunctions(){
    userDefinedFunctions.clear();
        NamedValueSet root = getRootObjectProperties();
    for(int i = 0 ; i < root.size() ; i++ ){
        // @ben only supported if move semantics are too are they for you?
        userDefinedFunctions.add(FunctionIdentifier(root.getName(i).toString()));
    }

}

void JsEnvironment::parameterValueChanged(Parameter * p) {
    callFunction("on_"+getJsFunctionNameFromAddress(p->getControlAddress()), p->value);

};
void JsEnvironment::triggerTriggered(Trigger *p){
    callFunction("on_"+getJsFunctionNameFromAddress(p->getControlAddress()), var::undefined());

}
