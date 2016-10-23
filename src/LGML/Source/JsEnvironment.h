/*
 ==============================================================================

 JsEnvironnement.h
 Created: 5 May 2016 9:03:35am
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef JAVASCRIPTENVIRONNEMENT_H_INCLUDED
#define JAVASCRIPTENVIRONNEMENT_H_INCLUDED

#include "JsGlobalEnvironment.h"



/// TODO need to be able to clean root name space user functions are never cleaned
// may be by recreating a new JsEngine
class JsEnvironment : public MultiTimer, //timer for autoWatch & timer for calling update() in scripts
public Parameter::Listener,
public Trigger::Listener ,
public ControllableContainerListener
{
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
  virtual void    clearNamespace();

  void    loadFile(const String & path);
  void    loadFile(const File & f);
  Result 	loadScriptContent(const String &content);

  void    reloadFile();
  void    showFile();


  String printAllNamespace();

  class Listener{
  public:
    virtual ~Listener(){};
    // listeners can check new namespaces from this function
    virtual void newJsFileLoaded(bool ) {};
    virtual void jsScriptLoaded(bool ) {};

  };
  void addListener(Listener * l){jsListeners.add(l);}
  void removeListener(Listener * l){jsListeners.remove(l);}
  void setAutoWatch(bool );

  int onUpdateTimerInterval;

  protected :


  // this firstCheck if function exists to avoid poluting Identifier global pool
  var callFunction (const String& function, const Array<var>& args, bool logResult = true,Result * =nullptr);
  var callFunction (const String& function, const var& args,  bool logResult = true,Result * =nullptr);

  var callFunctionFromIdentifier  (const Identifier& function, const Array<var> & args,bool logResult = true, Result* result=nullptr);
  var callFunctionFromIdentifier (const Identifier& function, const var & arg,bool logResult = true, Result* result = nullptr);


  static DynamicObject * getGlobalEnv(){return JsGlobalEnvironment::getInstance()->getEnv();}
  DynamicObject * getLocalEnv(){return localEnvironment.getDynamicObject();}


  // module name is the last element of dot separated localNamespace
  String getModuleName();
  String getParentName();
    String getCurrentFilePath(){return       currentFile.getRelativePathFrom(File::getCurrentWorkingDirectory());}//currentFile.getFullPathName();};


  const NamedValueSet & getRootObjectProperties();


  bool functionIsDefined(const String &);
  File currentFile;

  protected :
  Array<WeakReference<Parameter> > listenedParameters;
  Array<WeakReference<Trigger> > listenedTriggers;
  Array<WeakReference<ControllableContainer> > listenedContainers;
  void sendAllParametersToJS();
private:




  void    addToLocalNamespace(const String & elem,DynamicObject *target);
  void    removeNamespace(const String & jsNamespace);

  // dot separated string representing localNamespace
  String localNamespace;


  ListenerList<Listener> jsListeners;

  var localEnvironment;
  void internalLoadFile(const File &);


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


  class FunctionIdentifier{
  public:
    FunctionIdentifier(const String & s)
    {
      StringArray arr;
      arr.addTokens(s,"_","");
      for(auto & ts : arr.strings){
        splitedName.add(ts);
      }
    };
    bool compare(const String & s){
      StringArray arr;
      arr.addTokens(s,"_","");
      return compare(arr);
    }
    bool compare(const StringArray & arr){
      if(arr.size() != splitedName.size()){
        return false;
      }
      for(int i = 0 ; i < arr.size() ; i++){
        if(splitedName[i] != arr[i]){
          return false;
        }
      }
      return true;

    }
    Array<String> splitedName;
  };
  Array<FunctionIdentifier> userDefinedFunctions;

  ScopedPointer<JavascriptEngine> jsEngine;
  CriticalSection engineLock;

  void timerCallback(int timerID)override;
  Time lastFileModTime;

  void checkUserControllableEventFunction();
  void parameterValueChanged(Parameter * c) override;
  void triggerTriggered(Trigger * p) override;

  void controllableFeedbackUpdate(ControllableContainer *originContainer,Controllable *)     override;
};






#endif  // JAVASCRIPTENVIRONNEMENT_H_INCLUDED
