/*
 ==============================================================================

 JavaScriptController.h
 Created: 5 May 2016 10:15:48am
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef JAVASCRIPTCONTROLLER_H_INCLUDED
#define JAVASCRIPTCONTROLLER_H_INCLUDED


#include "OSCDirectController.h"
#include "JsEnvironment.h"


class JsOSCListener;


class JavascriptController : public OSCDirectController ,public JsEnvironment{
public:
    JavascriptController();
    ~JavascriptController();
    Result processMessageInternal(const OSCMessage &m) override;
    Result callForMessage(const OSCMessage & msg);
    void callonAnyMsg(const OSCMessage & msg);

    static var sendOSCFromJS(const juce::var::NativeFunctionArgs& a);
    void onContainerParameterChanged(Parameter * p) override;
    void onContainerTriggerTriggered(Trigger * t ) override;



    void newJsFileLoaded()override;
    ControllerUI * createUI() override;


  static var createJsOSCListener(const var::NativeFunctionArgs & a);
  OwnedArray<JsOSCListener,CriticalSection> jsOSCListeners;

  static var OSCArgumentToVar(OSCArgument & a);
  

private:
    void buildLocalEnv() override;
  void clearNamespace()override;

  DynamicObject *  createOSCJsObject();


    StringParameter * jsPath;
};



class JsOSCListener{
  public :
  JsOSCListener(JsEnvironment * js, OSCAddressPattern & pattern):jsEnv(js),
  addressPattern(pattern){
    buildVarObject();

  }
  static Identifier oscReceivedCallbackId;


  virtual ~JsOSCListener(){};
  void buildVarObject(){
    object= new DynamicObject();
    DynamicObject * dob = object.getDynamicObject();
    dob->setMethod(oscReceivedCallbackId,&JsOSCListener::dummyCallback);

  }

  // overriden in Js
#pragma warning(push)
#pragma warning(disable:4305 4800)
  static var dummyCallback(const var::NativeFunctionArgs &){
	  return var::undefined;
  };
#pragma warning(pop)
  void processMessage(const OSCMessage & msg){
    if(addressPattern == msg.getAddressPattern()){
      StringArray adList;
      adList.addTokens(msg.getAddressPattern().toString(),"/","");
      if(adList.size())adList.remove(0);

      var jsArgs[2];
      var * addressList = &jsArgs[0];
      for(auto &a:adList){addressList->append(a);};
      var * argsList = &jsArgs[1];
      if(msg.size()==1){*argsList = JavascriptController::OSCArgumentToVar(msg[0]);}
      else if (msg.size()>1){
        for(auto & m:msg){argsList->append(JavascriptController::OSCArgumentToVar(m));}
      }

      jsEnv->callFunctionFromIdentifier(oscReceivedCallbackId, var::NativeFunctionArgs(object,jsArgs,2), true);
    }
  };
  JsEnvironment* jsEnv;
  OSCAddressPattern  addressPattern;
  var object;
};


#endif  // JAVASCRIPTCONTROLLER_H_INCLUDED
