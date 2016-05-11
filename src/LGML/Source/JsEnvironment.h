/*
 ==============================================================================

 JsEnvironnement.h
 Created: 5 May 2016 9:03:35am
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef JAVASCRIPTENVIRONNEMENT_H_INCLUDED
#define JAVASCRIPTENVIRONNEMENT_H_INCLUDED

#include "JuceHeader.h"
#include "JsGlobalEnvironment.h"

class JsEnvironment : public Timer,public Parameter::Listener,public Trigger::Listener {
public:
    JsEnvironment(const String & ns);
    virtual ~JsEnvironment();

    // should be implemented to build localenv
    virtual void buildLocalEnv() = 0;

    // sub classes can check new namespaces from this function
    virtual void newJsFileLoaded(){};

    // can check that if want to avoid unnecessary (and potentially unsecure) method calls on non-valid jsFile
    bool hasValidJsFile(){return _hasValidJsFile;}

    // dynamically update namespace name
    void    setNamespaceName(const String &);

    // helpers

    void    setLocalNamespace(DynamicObject & target);
    void    clearNamespace();

    void    loadFile(const File & f);
    void    reloadFile();
    void    showFile();


    String printAllNamespace();

    class Listener{
    public:
        virtual ~Listener(){};
        // listeners can check new namespaces from this function
        virtual void newJsFileLoaded(bool state)=0;

    };
    void addListener(Listener * l){jsListeners.add(l);}
    void removeListener(Listener * l){jsListeners.remove(l);}
    void setAutoWatch(bool );

protected :

    var callFunction  (const Identifier& function, const Array<var> & args, Result* result=nullptr);
    var callFunction (const Identifier& function, const var & arg, Result* result = nullptr);

    static DynamicObject * getGlobalEnv(){return JsGlobalEnvironment::getInstance()->getEnv();}
    DynamicObject * getLocalEnv(){return localEnvironment.getDynamicObject();}


    // module name is the last element of dot separated localNamespace
    String getModuleName();
    String getParentName();
    String getCurrentFilePath(){return currentFile.getFullPathName();};


    const NamedValueSet & getRootObjectProperties();



private:


    void    addToLocalNamespace(const String & elem,DynamicObject *target);
    void    removeNamespace(const String & jsNamespace);

    // dot separated string representing localNamespace
    String localNamespace;
    File currentFile;

    ListenerList<Listener> jsListeners;

    var localEnvironment;
    void internalLoadFile(const File &);

    bool _hasValidJsFile;
    
    
    JavascriptEngine jsEngine;

    void timerCallback()override;
    Time lastFileModTime;

    void checkUserControllableEventFunction();
    Array<WeakReference<Parameter> > listenedParameters;
    Array<WeakReference<Trigger> > listenedTriggers;
    void parameterValueChanged(Parameter * c) override;
    void triggerTriggered(Trigger * p) override;
};






#endif  // JAVASCRIPTENVIRONNEMENT_H_INCLUDED
