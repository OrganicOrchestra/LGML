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


#ifndef JAVASCRIPTENVIRONNEMENT_H_INCLUDED
#define JAVASCRIPTENVIRONNEMENT_H_INCLUDED
#include "../../Controllable/Parameter/ParameterContainer.h"

#include "JsHelpers.h"
// needed for T* castPtrFromJSEnv()

#include "../../Controllable/Parameter/FileParameter.h"



class JsParameterListenerObject;

class JSEnvContainer;

class JsEnvironment : public MultiTimer, //timer for autoWatch & timer for calling update() in scripts
    private ParameterBase::Listener,
private ControllableContainer::FeedbackListener

{
public:
    JsEnvironment (const String& ns, ParameterContainer* linkedContainer);
    virtual ~JsEnvironment();



    ScopedPointer<JSEnvContainer> jsParameters;
    FileParameter * getJsFileParameter();

    // should be implemented to build localenv
    virtual void buildLocalEnv() = 0;


    // can check that if want to avoid unnecessary (and potentially unsecure) method calls on non-valid jsFile
    bool hasValidJsFile() {return _hasValidJsFile;}
    bool isInSyncWithLGML() {return _isInSyncWithLGML;}

    // dynamically update namespace name
    void    setNamespaceName (const String&);

    // returns object in global scope (includes params)
    DynamicObject * getGlobalObject();
    // helpers

    void    setLocalNamespace (DynamicObject& target);
    virtual void    clearNamespace();
    void setScriptEnabled (bool t);


    

    Result    loadScriptContent (const String& content);
    
    
    




    String printAllNamespace();

    
    void setAutoWatch (bool );

    var callFunction (const String& function, const Array<var>& args, bool logResult = true, Result* = nullptr);
    var callFunction (const String& function, const var& args,  bool logResult = true, Result* = nullptr);

    var callFunctionFromIdentifier  (const Identifier& function, const Array<var>& args, bool logResult = true, Result* result = nullptr);
    var callFunctionFromIdentifier (const Identifier& function, const var& arg, bool logResult = true, Result* result = nullptr);
    var callFunctionFromIdentifier (const Identifier& function, const var::NativeFunctionArgs& Nargs, bool logResult = true, Result* result = nullptr);

    WeakReference<JsEnvironment >::SharedPointer* getMasterRefPtr(){return masterReference.getSharedPointer (this);}
private:
    WeakReference<JsEnvironment>::Master masterReference;
    friend class WeakReference<JsEnvironment>;

protected :

    Result    loadFile (const File& f);

    // this firstCheck if function exists to avoid poluting Identifier global pool

    static Identifier noFunctionLogIdentifier;

    static DynamicObject::Ptr getGlobalEnv();
    //  DynamicObject * getLocalEnv(){return localEnv;}
    DynamicObject::Ptr localEnv;

    friend class JsContainerSync;


    // module name is the last element of dot separated localNamespace
    String getModuleName();
    String getParentName();



    const NamedValueSet& getRootObjectProperties();


    bool functionIsDefined (const String&);
    bool functionIdentifierIsDefined (const Identifier& i);
    

    static var createParameterListenerObject (const var::NativeFunctionArgs& a);



    typedef HashMap<String ,WeakReference<ParameterBase> > ListenedParameterType ;
    ListenedParameterType listenedParameters;
    Array<WeakReference<ControllableContainer> > listenedContainers;
    void sendAllParametersToJS();


    struct JsTimerType
    {
        JsTimerType (const int _id, const int _interval): id (_id), interval (_interval) {}
        const int id;
        const int interval;
    };

    
    static const JsTimerType onUpdateTimer;
    void setTimerState (const JsTimerType& t, bool state);
    friend class Controller;

    friend class JSEnvContainer;

    



private:


    bool isEnabled;



    void    addToLocalNamespace (const String& elem, DynamicObject* target);
    void    removeNamespace (const String& jsNamespace);

    // dot separated string representing localNamespace
    String localNamespace;


    


    bool _hasValidJsFile;


    void updateUserDefinedFunctions();


    // store function name string as Array of Identifier i.e :on_myFunc_lala => ["on","MyFunc","lala"]
    // the first objective of this function is to offer a simple way to test if a function exists without creating Identifiers each time (can be unsecure if a lot of message are recieved in OSC case)
    // we can then compare them by elements to avoid polluting Identifier global stringpool exponnentially
    //    i.e: on_track0_rec / on_track0_clear / on_track0_pause / on_track0_play /
    //         on_track1_rec / on_track1_clear / on_track1_pause / on_track1_play /
    //         on_track2_rec / on_track2_clear / on_track2_pause / on_track2_play /
    //         on_track3_rec / on_track3_clear / on_track3_pause / on_track3_play /
    //         on_track4_rec / on_track4_clear / on_track4_pause / on_track4_play /

    // would need only 1+5+4 = 9 Strings instead of 16 and so on when possible combinations grows


    class FunctionIdentifier
    {
    public:
        FunctionIdentifier (const String& s)
        {
            splitedName.addTokens (s, "_", "");
            identifier = s;
        };

        bool compareIdentifier (const Identifier& i)
        {
            return i == identifier;
        }
        bool compare (const String& s) const
        {
            StringArray arr;
            arr.addTokens (s, "_", "");
            return compare (arr);
        }
        bool compare (const StringArray& arr) const
        {
            return compare (arr.strings);
        }

        bool compare (const Array<String>& arr) const
        {
            if (arr.size() != splitedName.size())
            {
                return false;
            }

            for (int i = 0 ; i < arr.size() ; i++)
            {
                if (splitedName[i] != arr[i]) {return false;}
            }

            return true;

        }
        bool operator== (const FunctionIdentifier& other) const
        {
            return compare (other.splitedName);
        }
        StringArray splitedName;
        Identifier identifier;
    };
    Array<FunctionIdentifier> userDefinedFunctions;

    ScopedPointer<JavascriptEngine> jsEngine;
    CriticalSection engineLock;

    void timerCallback (int timerID)override;

    

    void clearListeners();
    Result checkUserControllableEventFunction();
    void parameterValueChanged ( ParameterBase* c, ParameterBase::Listener * notifier=nullptr) override;


    void controllableFeedbackUpdate (ControllableContainer* originContainer, Controllable*)     override;
    void childStructureChanged (ControllableContainer*, ControllableContainer*,bool isAdded) override;

    WeakReference<ParameterContainer> linkedContainer;
    bool isLoadingFile;
    int triesToLoad;
    bool _isInSyncWithLGML;


    static Identifier onUpdateIdentifier;



    OwnedArray<JsParameterListenerObject> parameterListenerObjects;
    void addJsParameterListener(JsParameterListenerObject * p){parameterListenerObjects.add(p);}
    friend class JsParameterListenerObject;



};


template<class T>
T* castPtrFromJSEnv(const var::NativeFunctionArgs& a){
    return JsHelpers::castPtrFromJS<T,JsEnvironment>(a);
}





class JsParameterListenerObject: public ParameterBase::AsyncListener
{
public :
    JsParameterListenerObject (JsEnvironment* js, ParameterBase* p): jsEnv (js), parameter (p)
    {
        buildVarObject();

        if (parameter)
        {
            parameter->addAsyncParameterListener (this);
        }

        else
        {
            NLOGE(js->localNamespace, juce::translate("wrong Parameter Listener type for : ") + parameter->shortName);
        }
    }
    static Identifier parameterChangedFId;
    static Identifier parameterObjectId;

    virtual ~JsParameterListenerObject()
    {
        if (parameter.get()) parameter->removeAsyncParameterListener (this);
    };



    void buildVarObject()
    {
        object = new DynamicObject();

        if (parameter.get())
        {
            DynamicObject* dob = object.getDynamicObject();
            dob->setProperty (parameterObjectId, parameter->createDynamicObject());
            dob->setMethod (parameterChangedFId, &JsParameterListenerObject::dummyCallback);

        }
    }

    // overriden in Js

    static var dummyCallback (const var::NativeFunctionArgs& /*a*/) {return var::undefined();};

    void newMessage (const ParameterBase::ParamWithValue& pv)override
    {
        jsEnv->callFunctionFromIdentifier (parameterChangedFId, var::NativeFunctionArgs (object, &pv.value, 1), true);

    };


    JsEnvironment* jsEnv;
    WeakReference<ParameterBase> parameter;
    var object;
};


//////////////////
/// JSEnvContainer

class JSEnvContainer : public ParameterContainer
{
public:
    JSEnvContainer (JsEnvironment* pEnv);
    ~JSEnvContainer();
    

    void onContainerParameterChanged ( ParameterBase* p) override;
    void onContainerTriggerTriggered (Trigger* p)override;
    FileParameter* scriptPath;

    Trigger* logT;

    JsEnvironment* jsEnv;

private:
    WeakReference<JSEnvContainer>::Master masterReference;
    friend class WeakReference<JSEnvContainer>;
};


#endif  // JAVASCRIPTENVIRONNEMENT_H_INCLUDED
