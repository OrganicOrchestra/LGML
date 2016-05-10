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

class JsEnvironment {
public:
    JsEnvironment(const String & ns);
    virtual ~JsEnvironment();

    // should be implemented to uild localenv
    virtual void buildLocalEnv() = 0;
    // sub classes can check new namespaces from this function
    virtual void newJsFileLoaded(){};

    // can check that if want to avoid unnecessary (and potentially unsecure) method calls on non-valid jsFile
    bool hasValidJsFile(){return _hasValidJsFile;}

    void    setNameSpaceName(const String &);


    // helpers
    void    addToLocalNamespace(const String & elem,DynamicObject *target);
    void setLocalNamespace(DynamicObject & target);

    void    removeNamespace(const String & jsNamespace);
    void    loadFile(const File & f);
    void    reloadFile();
    void    showFile();

    static DynamicObject * getGlobalEnv(){return JsGlobalEnvironment::getInstance()->getEnv();}

    DynamicObject * getLocalEnv(){return localEnvironment.getDynamicObject();}
    String printAllNamespace();

    void clearNamespace();

    protected :
    // dot separated string representing localNamespace
    String localNamespace;
    // module name is the last element of dot separated localNamespace
    String getModuleName();
    String getParentName();
    File currentFile;

    // allow to call function
    var callFunction (const Identifier& function, const var::NativeFunctionArgs& args, Result* result);
    const NamedValueSet & getRootObjectProperties();
private:

    var localEnvironment;
    void internalLoadFile(const File &);
    
    bool _hasValidJsFile;


    JavascriptEngine jsEngine;
    
};




#endif  // JAVASCRIPTENVIRONNEMENT_H_INCLUDED
