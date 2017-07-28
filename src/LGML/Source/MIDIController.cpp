/*
 ==============================================================================

 MIDIController.cpp
 Created: 2 Mar 2016 8:51:20pm
 Author:  bkupe

 ==============================================================================
 */

#include "MIDIController.h"
#include "MIDIControllerEditor.h"

#include "MIDIManager.h"
#include "DebugHelpers.h"
#include "JsHelpers.h"

extern AudioDeviceManager &  getAudioDeviceManager();


const static Identifier usrMIDICCId("usrMIDICC");
MIDIController::MIDIController() :
Controller("MIDI"),JsEnvironment("controller.mIDI",this)
{
	setNamespaceName("controller."+shortName);
	deviceInName = addStringParameter("midiPortName", "name of Midi device input", "");
	logIncoming = addBoolParameter("logIncoming","log Incoming midi message",false);

	channelFilter = addIntParameter("Channel", "Channel to filter message (0 = accept all channels)",0,0,16);


	// TODO : we may need to listen to sR changes
	juce::AudioDeviceManager::AudioDeviceSetup setup;
	getAudioDeviceManager().getAudioDeviceSetup(setup);
	midiCollector.reset(setup.sampleRate);

	addMIDIListenerListener(this);

}

MIDIController::~MIDIController()
{
	removeMIDIListenerListener(this);
	setCurrentDevice(String::empty);
  
}

void MIDIController::currentDeviceChanged(MIDIListener *)
{
	if (midiPortName != deviceInName->stringValue())
	{
		deviceInName->setValue(midiPortName, true);
	}
}

ControllerEditor *  MIDIController::createEditor(){
  return new MIDIControllerEditor(this );
}

void MIDIController::handleIncomingMidiMessage (MidiInput* ,
                                                const MidiMessage& message)
{

  if (!enabledParam->boolValue()) return;

	if (channelFilter->intValue() > 0 && message.getChannel() != channelFilter->intValue())
	{
		return;
	}

    if(message.isController()){
      if (logIncoming->boolValue())
      {
        NLOG("MIDI","CC "+String(message.getControllerNumber()) + " > " + String(message.getControllerValue())+" (Channel "+String(message.getChannel())+")");
      }

      ControllableContainer::UsrParameterList * usrMIDICC = getUserParameters(usrMIDICCId);
        for(Controllable* c:*usrMIDICC){
          if(((Parameter*)c)->intValue()==1){

          }
//        message.
//      int variableIndex = 128 + message.getControllerNumber() -1;
      //DBG("Variable name " << variables[variableIndex]->parameter->niceName);
//      variables[variableIndex]->setValue(message.getControllerValue()*1.f / 127.f);
      }

    }
    else if(message.isNoteOnOrOff()){
      if (logIncoming->boolValue())
      {
        NLOG("MIDI", "Note " + String(message.isNoteOn() ? "on" : "off") + " : " + MidiMessage::getMidiNoteName(message.getNoteNumber(), true, true, 0) + " > " + String(message.getVelocity()) + " (Channel " + String(message.getChannel()) + ")");
      }
//      int variableIndex = message.getNoteNumber();
//      variables[variableIndex]->setValue(message.isNoteOn()?(message.getVelocity()*1.f / 127.f):0);
    }

    else if (message.isPitchWheel()){
      if (logIncoming->boolValue())
      {
        NLOG("MIDI", "pitch wheel " + String(message.getPitchWheelValue()));
      }
    }
    else{
      if (logIncoming->boolValue())
      {
      NLOG("MIDI","message : "+message.getDescription());
      }
    }

  if(message.isNoteOff()){
    callJs(MidiMessage::noteOff(message.getChannel(), message.getNoteNumber(), 0.0f));
  }
  else{
    callJs(message);
  }
    activityTrigger->trigger();
}




void MIDIController::callJs(const MidiMessage& message){
  if(message.isController()){
    static const Identifier onCCFunctionName("onCC");
    Array<var> args;
    args.add(message.getControllerNumber());
    args.add(message.getControllerValue());
    callFunctionFromIdentifier(onCCFunctionName, args);
    for(auto & p:jsCCListeners){
      p->processMessage(message);
    }
  }
  else if(message.isNoteOnOrOff()){
    static const Identifier onCCFunctionName("onNote");
    Array<var> args;
    args.add(message.getNoteNumber());
    args.add(message.isNoteOn()?message.getVelocity():0);
    callFunctionFromIdentifier(onCCFunctionName, args);
    for(auto & p:jsNoteListeners){
      p->processMessage(message);
    }
  }
  else if(message.isPitchWheel()){
    static const Identifier onPitchWheelFunctionName("onPitchWheel");
    Array<var> args;
    args.add(message.getPitchWheelValue());
    callFunctionFromIdentifier(onPitchWheelFunctionName, args);
  }
}

void MIDIController::onContainerParameterChanged(Parameter * p){
  Controller::onContainerParameterChanged(p);
  if(p==nameParam){
    setNamespaceName("controller."+shortName);
  }
  else if (p==deviceInName){
    setCurrentDevice(deviceInName->stringValue());
  }
}

void MIDIController::buildLocalEnv(){
  DynamicObject obj;

  static const Identifier jsSendNoteOnIdentifier("sendNoteOn");
  obj.setMethod(jsSendNoteOnIdentifier, sendNoteOnFromJS);

  static const Identifier jsSendNoteOffIdentifier("sendNoteOff");
  obj.setMethod(jsSendNoteOffIdentifier, sendNoteOffFromJS);

  static const Identifier jsSendCCIdentifier("sendCC");
  obj.setMethod(jsSendCCIdentifier, sendCCFromJS);

  static const Identifier jsSendSysExIdentifier("sendSysEx");
  obj.setMethod(jsSendSysExIdentifier, sendSysExFromJS);

  static const Identifier jsGetCCListenerObject("createCCListener");
  obj.setMethod(jsGetCCListenerObject, &MIDIController::createJsCCListener);

  static const Identifier jsGetNoteListenerObject("createNoteListener");
  obj.setMethod(jsGetNoteListenerObject, &MIDIController::createJsNoteListener);

  obj.setProperty(jsPtrIdentifier, (int64)this);
  setLocalNamespace(obj);



};


void MIDIController::newJsFileLoaded(){

  
};

// @ben do we do the same as OSC MIDI IN/OUt controllers

var MIDIController::sendNoteOnFromJS(const var::NativeFunctionArgs & a ) {

  MIDIController * c = getObjectPtrFromJS<MIDIController>(a);

  if (a.numArguments < 3)
  {
    NLOG("MidiController", "Incorrect number of arguments for sendNoteOn");
    return var(false);
  }

  c->sendNoteOn((int)(a.arguments[0]) , a.arguments[1], a.arguments[2]);
  return var(true);
}


var MIDIController::sendNoteOffFromJS(const var::NativeFunctionArgs & a) {

  MIDIController * c = getObjectPtrFromJS<MIDIController>(a);

  if (a.numArguments < 3)
  {
    NLOG("MidiController", "Incorrect number of arguments for sendNoteOff");
    return var(false);
  }

  c->sendNoteOff((int)(a.arguments[0]), a.arguments[1], a.arguments[2]);
  return var(true);
}

var MIDIController::sendCCFromJS(const var::NativeFunctionArgs & a){
  MIDIController * c = getObjectPtrFromJS<MIDIController>(a);

  if (a.numArguments < 3)
  {
    NLOG("MidiController", "Incorrect number of arguments for sendCC");
    return var(false);
  }

  c->sendCC((int)(a.arguments[0]) , a.arguments[1], a.arguments[2]);
  return var(true);
}

var MIDIController::sendSysExFromJS(const var::NativeFunctionArgs & a)
{
  MIDIController * c = getObjectPtrFromJS<MIDIController>(a);

  if (a.numArguments > 8)
  {
    NLOG("MidiController", "Incorrect number of arguments for sendSysEx");
    return var(false);
  }
  uint8 bytes[8];
  for (int i = 0; i < a.numArguments; i++)
  {
    
    bytes[i] = (uint8)(int)a.arguments[i];
    LOG("Byte : " << String(bytes[i]));
  }
  c->sendSysEx(bytes,a.numArguments);
  return var(true);
}


var MIDIController::createJsNoteListener(const var::NativeFunctionArgs & a){
  if(a.numArguments<2){ return var::undefined();}

  int channel = a.arguments[0];
  int numberToListen = a.arguments[1];
  MIDIController * originEnv = dynamic_cast<MIDIController*>(a.thisObject.getDynamicObject());
    if(originEnv){
      JsMIDIMessageListener * ob = new JsMIDIMessageListener(originEnv,channel,numberToListen,true);
      originEnv->jsNoteListeners.add(ob);
      return ob->object;
    }
  
  return var::undefined();
}
var MIDIController::createJsCCListener(const var::NativeFunctionArgs & a){
  if(a.numArguments<2){ return var::undefined();}

  int channel = a.arguments[0];
  int numberToListen = a.arguments[1];
  MIDIController * originEnv = dynamic_cast<MIDIController*>(a.thisObject.getDynamicObject());
  if(originEnv){
    JsMIDIMessageListener * ob = new JsMIDIMessageListener(originEnv,channel,numberToListen,false);
    originEnv->jsCCListeners.add(ob);
    return ob->object;
  }

  return var::undefined();
}

void MIDIController::clearNamespace(){
  JsEnvironment::clearNamespace();
  {
    const ScopedLock lk (jsNoteListeners.getLock());
    jsNoteListeners.clear();
  }
  {
    const ScopedLock lk (jsCCListeners.getLock());
    jsCCListeners.clear();
  }
}
////////
// MidiListener


Identifier JsMIDIMessageListener::midiReceivedId("onMidi");
Identifier JsMIDIMessageListener::midiValueId("value");
