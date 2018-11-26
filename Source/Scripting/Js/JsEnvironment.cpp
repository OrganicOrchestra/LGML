/* Copyright © Organic Orchestra, 2017
 *
 * This file is part of LGML.  LGML is a software to manipulate sound in realtime
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation (version 3 of the License).
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */


#include "JsEnvironment.h"
#include "../../Time/TimeManager.h"
#include "../../Node/Manager/NodeManager.h"
#include "../../Controller/ControllerManager.h"
#include "../../Utils/DebugHelpers.h"


#include "../../Engine.h"
#include "JsGlobalEnvironment.h"




Identifier JsEnvironment::noFunctionLogIdentifier ("no function Found");
Identifier JsEnvironment::onUpdateIdentifier ("onUpdate");

const JsEnvironment::JsTimerType JsEnvironment::autoWatchTimer (0, 500);
const JsEnvironment::JsTimerType JsEnvironment::onUpdateTimer (1, 20);


DynamicObject::Ptr JsEnvironment::getGlobalEnv() {return JsGlobalEnvironment::getInstance()->getEnv();}

JsEnvironment::JsEnvironment (const String& ns, ParameterContainer* _linkedContainer) :
linkedContainer (_linkedContainer),
localNamespace (ns),
_hasValidJsFile (false),
autoWatch (false),
_isInSyncWithLGML (false),
isLoadingFile (false),
isEnabled (true)

{
    jsParameters = new JSEnvContainer (this);
    linkedContainer->addChildControllableContainer (jsParameters);
    linkedContainer->nameParam->addParameterListener(this);
    localEnv = new DynamicObject();
    clearNamespace();
    getEngine()->addControllableContainerListener (this);
    //  addToNamespace(localNamespace, localEnv, getGlobalEnv());


    triesToLoad = 5;

}

JsEnvironment::~JsEnvironment()
{
    if (getEngine())
    {
        getEngine()->removeControllableContainerListener (this);
    }

    if(linkedContainer.get()){
        linkedContainer->nameParam->removeParameterListener(this);
    }
    stopTimer (autoWatchTimer.id);
    stopTimer (onUpdateTimer.id);
    clearListeners();


    if (localEnv)
    {localEnv->clear();}
    JsEnvironment::masterReference.clear();



}

void JsEnvironment::setScriptEnabled (bool t)
{
    if (t == isEnabled) {return;}

    if (t)
    {
        loadFile (currentFile);

        if (getEngine()) {getEngine()->addControllableContainerListener (this);}

    }
    else
    {
        if (getEngine()) {getEngine()->removeControllableContainerListener (this);}

        clearNamespace();
        clearListeners();
    }

    setTimerState (onUpdateTimer, t);

    isEnabled = t;
}

DynamicObject* JsEnvironment::getGlobalObject(){
    String ns = localNamespace;
    if(ns.length())
        return JsGlobalEnvironment::getInstance()->getNamespaceObject(ns).get();
    else
        return nullptr;
}

var getLocal(const juce::var::NativeFunctionArgs &a){
    auto c = JsHelpers::castPtrFromJS<JsEnvironment> (a);
    if (c != nullptr)
    {
        return c->getGlobalObject();
    }
    else
    {
        LOGE(juce::translate("unknown controllable set from js"));
        jassertfalse;
    }

    return var();
}


void JsEnvironment::clearNamespace()
{
    const  ScopedLock lk (engineLock);

    JsHelpers::clearRefsFromObj(localEnv);
    localEnv->clear();

    jsEngine = new JavascriptEngine();


    static Identifier createParamListenerId ("createParameterListener");
    localEnv->setMethod (createParamListenerId, &JsEnvironment::createParameterListenerObject);
//    localEnv->setProperty (jsPtrIdentifier, -1);//(int64)(JsEnvironment*)this);
    if(linkedContainer.get()){
        localEnv->setMethod("getLocal",&getLocal );
    }
    jsEngine->registerNativeObject (JsHelpers::jsLocalIdentifier, localEnv);
    jsEngine->registerNativeObject (JsHelpers::jsGlobalIdentifier, getGlobalEnv());





}


void JsEnvironment::removeNamespace (const String& jsNamespace)
{
    JsHelpers::removeNamespaceFromObject (jsNamespace, localEnv);
}

String JsEnvironment::getParentName()
{
    int idx = localNamespace.indexOfChar ('.');
    return localNamespace.substring (0, idx);
}


bool JsEnvironment::loadFile (const String& path)
{

    File f = getEngine()->getFileAtNormalizedPath (path);
    return loadFile (f);

}

bool JsEnvironment::loadFile (const File& f)
{
    if (!f.existsAsFile() || f.getFileExtension() != ".js")
    {
        //    jsParameters->scriptPath->setValue("");
        clearNamespace();
        _hasValidJsFile = false;
        _isInSyncWithLGML = false;
        buildLocalEnv();
        clearListeners();

        jsListeners.call (&JsEnvironment::Listener::newJsFileLoaded, false);
        return false;
    }

    currentFile = f;

    if (isEngineLoadingFile())
    {
        startTimer (autoWatchTimer.id, autoWatchTimer.interval);
        return true;
    }

    internalLoadFile (f);

    if (!autoWatch) stopTimer (autoWatchTimer.id);

    return true;


}

void JsEnvironment::reloadFile()
{
    if (!currentFile.existsAsFile())return;

    triesToLoad = 5;
    internalLoadFile (currentFile);
}

void JsEnvironment::showFile()
{
    if (!currentFile.existsAsFile())return;

    currentFile.startAsProcess();
}

const File& JsEnvironment::getCurrentFile() { return currentFile;}

String JsEnvironment::getCurrentFilePath() { return getEngine()->getNormalizedFilePath (currentFile);}

void JsEnvironment::internalLoadFile (const File& f )
{

    if (triesToLoad <= 0) {stopTimer (autoWatchTimer.id); return;}

    StringArray destLines;
    f.readLines (destLines);
    String jsString = destLines.joinIntoString ("\n");


    Result r = loadScriptContent (jsString);


    static FunctionIdentifier onUpdateFId (onUpdateIdentifier.toString());

    if (r.failed() && !_isInSyncWithLGML) {triesToLoad--;}
    else {_isInSyncWithLGML = true;}

    String normalizedPath =  getCurrentFilePath();
    jsParameters->scriptPath->setValueFrom (jsParameters,normalizedPath);
    jsListeners.call (&JsEnvironment::Listener::newJsFileLoaded, (bool)r);

    // TODO get rid of this once unifying JsEnvironment
    //bool isEnabled = (bool)r && _hasValidJsFile && _isInSyncWithLGML;
    if (Controller* cc = dynamic_cast<Controller*> (this))
    {
        isEnabled = cc->enabledParam->boolValue();
    }
    JsGlobalEnvironment::getInstance()->setControllableContainerDirty(linkedContainer);
    setTimerState (onUpdateTimer, isEnabled && userDefinedFunctions.contains (onUpdateFId) );
}

Result JsEnvironment::loadScriptContent (const String& content)
{
    // rebuild to clean namespace
    clearNamespace();
    buildLocalEnv();
    clearListeners();
    // thread safe if the  environment class is not multithreaded
    Result r = Result::fail ("can't lock environment");
    {
        const ScopedLock lk (engineLock);
        r = jsEngine->execute (content);
    }


    if (r.failed())
    {
        _hasValidJsFile = false;
        //        jsEngine = nullptr;
        // NLOG(localNamespace,printAllNamespace());
        NLOGE (localNamespace, r.getErrorMessage());
    }
    else
    {
        _hasValidJsFile = true;
        lastFileModTime = currentFile.getLastModificationTime();
        updateUserDefinedFunctions();
        r = checkUserControllableEventFunction();
        _isInSyncWithLGML = (bool)r;
        newJsFileLoaded();

        if (_isInSyncWithLGML)
        {
            NLOG (localNamespace, juce::translate("Content loaded sucessfully"));
        }
        else
        {
            NLOGE (localNamespace,  r.getErrorMessage());
        }
    }

    jsListeners.call (&JsEnvironment::Listener::jsScriptLoaded, (bool)r);

    return r;
}

void JsEnvironment::clearListeners()
{
    ListenedParameterType::Iterator it(listenedParameters);
    while (it.next())
    {
        auto c = it.getValue();
        if (c.get()) c->removeParameterListener (this);
    }

    listenedParameters.clear();

    for (auto& c : listenedContainers)
    {
        if (c.get())c->removeControllableContainerListener (this);
    }

    listenedContainers.clear();

    {
        const ScopedLock lk (engineLock);
        parameterListenerObjects.clear();
    }

}

bool JsEnvironment::functionIsDefined (const juce::String& s)
{
    StringArray arr;
    arr.addTokens (s, "_", "");

    for (auto& f : userDefinedFunctions)
    {
        if (f.compare (arr)) {return true;}
    }

    return false;
}

bool JsEnvironment::functionIdentifierIsDefined (const Identifier& i)
{
    for (auto& f : userDefinedFunctions)
    {
        if (f.compareIdentifier (i)) {return true;}
    }

    return false;
}



var JsEnvironment::callFunction (const String& function, const Array<var>& args, bool logResult, Result* result)
{

    if (!functionIsDefined (function))
    {

        if (result != nullptr)result->fail (noFunctionLogIdentifier.toString());

        if(logResult) NLOGE (localNamespace, noFunctionLogIdentifier.toString());

        return var::undefined();
    }

    return callFunctionFromIdentifier (function, args, logResult, result);
}

var JsEnvironment::callFunction (const String& function, const var& args, bool logResult, Result* result)
{

    if (!functionIsDefined (function))
    {
        if (result != nullptr)result->fail (noFunctionLogIdentifier.toString());

        if (logResult)NLOGE (localNamespace, noFunctionLogIdentifier.toString());

        return var::undefined();
    }

    return callFunctionFromIdentifier (function, args, logResult, result);
}



var JsEnvironment::callFunctionFromIdentifier (const Identifier& function, const Array<var>& args, bool logResult, Result* result)
{
    // force Native function to explore first level global scope by setting Nargs::thisObject to undefined
    var v;
    juce::var::NativeFunctionArgs Nargs (var::undefined(), (args.size() > 0) ? &args.getReference (0) : &v, args.size());

    return callFunctionFromIdentifier (function, Nargs, logResult, result);
}

var JsEnvironment::callFunctionFromIdentifier (const Identifier& function, const var& arg, bool logResult, Result* result)
{
    // force Native function to explore first level global scope by setting Nargs::thisObject to undefined
    juce::var::NativeFunctionArgs Nargs (var::undefined(), &arg, 1);
    return callFunctionFromIdentifier (function, Nargs, logResult, result);
}

#pragma warning(push)
#pragma warning(disable:4305 4800)
var JsEnvironment::callFunctionFromIdentifier (const Identifier& function, const var::NativeFunctionArgs& Nargs, bool logResult, Result* result)
{
    if (!hasValidJsFile()) return &var::undefined;

    bool resOwned = false;

    if (logResult && result == nullptr)
    {
        result = new Result (Result::ok());
        resOwned = true;
    }

    var res;
    {

        // TODO : assure thread safety (this one liner is non-atomic, but we may need check if engine is not locked)
        bool engineIsAlreadyLocked =  !ScopedTryLock (engineLock).isLocked();

        if (!engineIsAlreadyLocked)
        {

            if (auto je = JsGlobalEnvironment::getInstanceWithoutCreating())
                if(je->isDirty())
                {
                    res = jsEngine->callFunction (function, Nargs, result);
                }
            //            else
//            {
//                //      DBG("JS avoiding to call function while global environment is dirty");
//            }
        }
        else
        {
            //      jassertfalse;
            NLOGE (localNamespace, juce::translate("jsEngine is Locked"));
        }
    }

    if (logResult && result->failed())
    {
        NLOGE(localNamespace, result->getErrorMessage());
    }

    if (resOwned)
    {
        delete result;
        result = nullptr;
    }

    return res;
}
#pragma warning(pop)



const NamedValueSet& JsEnvironment::getRootObjectProperties()
{
    const ScopedLock lk (engineLock);
    return jsEngine->getRootObjectProperties();
}

void JsEnvironment::addToLocalNamespace (const String& elem, DynamicObject* target)
{
    JsHelpers::addToNamespace (elem, target, localEnv);
}

void JsEnvironment::setLocalNamespace (DynamicObject& target)

{
    clearNamespace();

    for (int i = 0; i < target.getProperties().size(); i++)
    {
        Identifier n = target.getProperties().getName (i);
        localEnv->setProperty (n, target.getProperty (n));
    }

    JsHelpers::assignPtrToObject(this,localEnv);

}

void JsEnvironment::setNamespaceName (const String& s)
{
    if (s != localNamespace)
    {
        DynamicObject* d = JsHelpers::getNamespaceFromObject (getParentName(), getGlobalEnv());
        jassert (d != nullptr);

        if (localEnv.get())
        {
            d->removeProperty (getModuleName());
            localNamespace = s;
            d->setProperty (getModuleName(), localEnv.get());
        }
    }
}
void JsEnvironment::setTimerState (const JsTimerType& t, bool s)
{
    if (s) {startTimer (t.id, t.interval);}
    else {stopTimer (t.id);}
}
void JsEnvironment::setAutoWatch (bool s)
{
    triesToLoad = 5;
    setTimerState (autoWatchTimer, s);
    autoWatch = s;
}

void JsEnvironment::timerCallback (int timerID)
{
    if (timerID == autoWatchTimer.id)
    {
        if (isEngineLoadingFile())return;

        Time newTime = currentFile.getLastModificationTime();

        if (newTime != lastFileModTime || !_isInSyncWithLGML )
        {

            isLoadingFile = true;

            if (! loadFile (currentFile))
            {
                stopTimer (autoWatchTimer.id);
            }

            isLoadingFile = false;
            lastFileModTime = newTime;
        }
    }
    else if (timerID == onUpdateTimer.id)
    {
        if (_hasValidJsFile && functionIdentifierIsDefined (onUpdateIdentifier))
        {
            callFunctionFromIdentifier (onUpdateIdentifier, var(), true);
        }
        else
        {
            stopTimer (onUpdateTimer.id);
        }
    }
}



String JsEnvironment::printAllNamespace()
{
    const ScopedLock lk (engineLock);
    return JsHelpers::namespaceToString (jsEngine->getRootObjectProperties(), 0, true, false);
}


String JsEnvironment::getModuleName()
{
    int idx = localNamespace.indexOfChar ('.');

    if (idx == -1) { return  localNamespace; }
    else { return localNamespace.substring (idx + 1); }
}



Result JsEnvironment::checkUserControllableEventFunction()
{
    Result res = Result::ok();

    NamedValueSet root = getRootObjectProperties();
    Array<FunctionIdentifier*> functionNamespaces;

    for (auto& f : userDefinedFunctions)
    {
        if (f.splitedName[0] == "on")functionNamespaces.add (&f);

    }

    static const Array<ControllableContainer*> candidates =
    {
        (ControllableContainer*)NodeManager::getInstance(),
        (ControllableContainer*)TimeManager::getInstance(),
        (ControllableContainer*)ControllerManager::getInstance()
    };



    for (auto& f : functionNamespaces)
    {
        bool found = false;

        if (f->splitedName.size() > 2)
        {
            for (auto& candidate : candidates)
            {
                if ((candidate->shortName == f->splitedName[1]))
                {
                    StringArray localName;

                    // remove on and candidate Name
                    for (int i = 2; i < f->splitedName.size(); i++)
                    {
                        localName.add (f->splitedName.strings.getUnchecked (i));
                    }

                    Controllable* c = candidate->getControllableForAddress (localName);

                    if ( ParameterBase* p = dynamic_cast <ParameterBase*> (c))
                    {
                        listenedParameters.set(f->splitedName.joinIntoString("_"),p);
                        found = true;
                        break;
                    }
                    else if (ControllableContainer* cont = candidate->getControllableContainerForAddress (localName))
                    {
                        listenedContainers.addIfNotAlreadyThere (cont);
                        found = true;
                        break;
                    }
                }

            }

        }

        if (!found)
        {
            String fName;

            for (auto& n : f->splitedName)
            {
                fName += n + "_";
            }

            fName = fName.substring (0, fName.length() - 1);

            if ( (bool)res)
            {
                res = Result::fail ("not found controllable/Container for function : " + fName);
            }
            else
            {
                res = Result::fail (res.getErrorMessage() + "," + fName);
            }

        }

    }
    ListenedParameterType::Iterator it(listenedParameters);
    while (it.next())
    {
        it.getValue()->addParameterListener (this);
    }

    for (auto& cont : listenedContainers)
    {
        cont->addControllableContainerListener (this);
    }

    return res;
}

void JsEnvironment::updateUserDefinedFunctions()
{
    userDefinedFunctions.clear();
    NamedValueSet root = getRootObjectProperties();

    for (int i = 0; i < root.size(); i++)
    {
        userDefinedFunctions.add (FunctionIdentifier (root.getName (i).toString()));
    }

}

void JsEnvironment::parameterValueChanged ( ParameterBase* p, ParameterBase::Listener * notifier)
{
    if (p == linkedContainer->nameParam)
    {
        // ensure short name is updated...
        // not sure it's needed though
        linkedContainer->setNiceName (linkedContainer->nameParam->stringValue());
        auto ns = linkedContainer->getControlAddressArray();
        if(ns.size()>=2){

            String namespaceName = ns.joinIntoString(".");
            setNamespaceName (namespaceName);
        }
        else{
            //            jassertfalse;
        }

    }

    else if (p){
        ListenedParameterType::Iterator it(listenedParameters);
        while (it.next())
        {
            if(it.getValue().get()==p){
                callFunction (it.getKey(),p->value, false);
            }
        }
    }
    else { jassertfalse; }

};

#define NON_BLOCKING 0
void JsEnvironment::controllableFeedbackUpdate (ControllableContainer* originContainer, Controllable* c)
{
    // avoid root callback (only used to reload if
    if (originContainer == getEngine())return;

    var v = var::undefined();

    if ( ParameterBase* p = dynamic_cast <ParameterBase*> (c))
        v = p->value;

    String address = c->getControlAddress (originContainer);
    StringArray sArr;
    sArr.addTokens (address, "/", "");
    jassert (sArr.size() > 0);
    sArr.remove (0);
    Array<var> add;

    for (auto& s : sArr) { add.add (s); }

    Array<var> argList = { var (add), v };

#if NON_BLOCKING
    auto f=[this,originContainer,argList](){
#endif
        callFunction ("on_" + JsHelpers::getJsFunctionNameFromAddress (originContainer->getControlAddress()), argList, false);
#if NON_BLOCKING
    };

    if(MessageManager::getInstance()->isThisTheMessageThread()){f();}
    else{MessageManager::callAsync(f);}
#endif
}


void JsEnvironment::childStructureChanged (ControllableContainer* notifierC, ControllableContainer* changed,bool isAdded)
{
    // rebuild files that are not loaded properly if LGML JsEnvironment is not fully built at the time of their firstExecution
    if ((!_hasValidJsFile || !_isInSyncWithLGML) && !isLoadingFile && notifierC == getEngine() && (linkedContainer.get() && !linkedContainer->containsContainer (changed)))
    {
        bool isFromOtherJsEnv = false;

        //    if(JsEnvironment *jsNotifier = dynamic_cast<JsEnvironment*>(notifier)){
        //      isFromOtherJsEnv = jsNotifier->isLoadingFile;
        //    }
        if (!isFromOtherJsEnv)
        {
            _isInSyncWithLGML = false;
            startTimer (autoWatchTimer.id, autoWatchTimer.interval);
        }
    };

};

void JsEnvironment::sendAllParametersToJS()
{

    ListenedParameterType::Iterator it(listenedParameters);
    while (it.next())
    {
        auto t = it.getValue();
        if (t.get())parameterValueChanged (t);
    }

    for (auto& t : listenedContainers)
    {
        if (t.get())
        {
            Array<WeakReference<Controllable>> conts = t->getAllControllables();

            for (auto& c : conts) { controllableFeedbackUpdate (t, c); }
        }
    }
}


var JsEnvironment::createParameterListenerObject (const var::NativeFunctionArgs& a)
{
    if (a.numArguments == 0) { return var::undefined(); }
    auto* originEnv =JsHelpers::castPtrFromJS<JsEnvironment> (a);
    if (originEnv)
    {

        DynamicObject* ob = a.arguments->getArray()->getUnchecked(0).getDynamicObject();
       if(ob) {
        auto*  p = JsHelpers::castPtrFromObject<ParameterBase>(ob) ;
        if ( p)
        {
                JsParameterListenerObject* ob = new JsParameterListenerObject (originEnv, p);
                originEnv->addJsParameterListener (ob);
                return ob->object;
        }

        auto  props =a.arguments[0].getDynamicObject()->getProperties();
        DBG(p->niceName << ", " << p->shortName);
        for(int i = 0 ; i < props.size() ; i++){
            DBG(props.getName(i).toString() << ":" << props.getValueAt(i).toString());
        }
       }
    }
    NLOGE("js","can't create parameterListener object");
    return var::undefined();
};




////////////////////////
// JSEnvContainer
//#include "JsEnvironmentUI.h"

JSEnvContainer::JSEnvContainer (JsEnvironment* pEnv):
ParameterContainer ("jsParams"), jsEnv (pEnv)
{

    scriptPath = addNewParameter<StringParameter> ("ScriptPath", "path for js script", "");
    scriptPath->isControllableExposed = false;
    loadT =  addNewParameter<Trigger> ("loadFile", "open dialog to choose file to load");
    loadT->isControllableExposed = false;
    reloadT =  addNewParameter<Trigger> ("reloadFile", "reload current file");
    reloadT->isControllableExposed = false;
    showT =  addNewParameter<Trigger> ("showFile", "open file in text editor");
    showT->isControllableExposed = false;
    autoWatch = addNewParameter<BoolParameter> ("autoWatch", "auto reload if file has been changed", false);
    autoWatch->isSavable = false;
    logT =  addNewParameter<Trigger> ("LogEnvironment", "print hierarchy of JS objects");
    createT = addNewParameter<Trigger> ("create", "create new script");

}

JSEnvContainer::~JSEnvContainer(){
    JSEnvContainer::masterReference.clear();
}

void JSEnvContainer::onContainerParameterChanged ( ParameterBase* p)
{
    if (p == scriptPath)
    {

        if (!jsEnv->loadFile (scriptPath->stringValue()) )
        {

        }
    }

    else if (p == autoWatch)
    {
        jsEnv->setAutoWatch (autoWatch->boolValue());
    }

}
void JSEnvContainer::onContainerTriggerTriggered (Trigger* p)
{
    if (p == showT)
    {
        jsEnv->showFile();
    }
    else if (p == reloadT)
    {
        jsEnv->reloadFile();
    }
    else if (p == logT)
    {
        LOG (jsEnv->printAllNamespace());
    }
    else if (p == loadT)
    {
        File  startFolder (jsEnv->getCurrentFile());

        if (startFolder.exists()) {startFolder = startFolder.getParentDirectory();}
        else {startFolder = getEngine()->getCurrentProjectFolder();}

        FileChooser myChooser ("Please select the script you want to load...",
                               startFolder,
                               "*.js");

        if (myChooser.browseForFileToOpen())
        {
            File script (myChooser.getResult());
            jsEnv->loadFile (script);
        }
    }
    else if (p==createT){

        if(!getEngine()->getCurrentProjectFolder().exists()){
            FileChooser fc("where to save the script?");
            if(fc.browseForFileToSave(true)){
                File scriptFile = fc.getResult();
                if(scriptFile.exists()){
                    scriptPath->setValue( scriptFile.getFullPathName());
                    showT->trigger();
                }
            }
            else{
                return;
            }
        }
        else{
            AlertWindow nameWindow ("Create script", "name your script", AlertWindow::AlertIconType::NoIcon);
            nameWindow.addTextEditor ("Name", "MyScript");

            nameWindow.addButton ("OK", 1, KeyPress (KeyPress::returnKey));
            nameWindow.addButton ("Cancel", 0, KeyPress (KeyPress::escapeKey));

            int result = nameWindow.runModalLoop();

            if (result)
            {
                String sN = nameWindow.getTextEditor("Name")->getText();
                if(sN.isNotEmpty()){
                    if( sN.endsWith(".js")){
                        sN= sN.substring(0, -2);
                    }
                    File scriptFile = getEngine()->getCurrentProjectFolder().getChildFile("Scripts/");

                    if(! scriptFile.exists()){
                        scriptFile.createDirectory();
                    }
                    scriptFile = scriptFile.getNonexistentChildFile(sN,".js",true);
                    if(! scriptFile.exists()){
                        auto r = scriptFile.create();
                        if(!r){
                            LOGE(juce::translate("Can't create script : ") <<scriptFile.getFullPathName() << "("<< r.getErrorMessage()<<")");
                        }
                    }
                    else{
                        jassertfalse;
                    }
                    
                    if(scriptFile.exists()){
                        scriptPath->setValue( scriptFile.getFullPathName());
                        showT->trigger();
                    }
                }
                
            }
        }
    }
}

//////////////////////
// JsParameterListenerObject


Identifier JsParameterListenerObject::parameterChangedFId ("parameterChanged");
Identifier JsParameterListenerObject::parameterObjectId ("parameter");
