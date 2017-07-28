/*
 ==============================================================================

 JavascriptController.h
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
    


  static var createJsOSCListener(const var::NativeFunctionArgs & a);
  OwnedArray<JsOSCListener,CriticalSection> jsOSCListeners;

  static var OSCArgumentToVar(OSCArgument & a);
  

private:
    void buildLocalEnv() override;
  void clearNamespace()override;

  DynamicObject *  createOSCJsObject();


    StringParameter * jsPath;
};


// jsObject to bind functions to OSC address from within js
//
//example.js
//
// var myListener = createOSCListener("/osc/pattern/to/watch")
//
// myListener.onOSC= function(value){
// gets activated when /osc/pattern/to/watch has been recieved
// do stuff with value...
// }
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
	  return &var::undefined;
  };

#pragma warning(pop)


  void processMessage(const OSCMessage & msg);


  JsEnvironment* jsEnv;
  OSCAddressPattern  addressPattern;
  var object;
};


#endif  // JAVASCRIPTCONTROLLER_H_INCLUDED
