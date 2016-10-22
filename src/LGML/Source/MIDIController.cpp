/*
 ==============================================================================

 MIDIController.cpp
 Created: 2 Mar 2016 8:51:20pm
 Author:  bkupe

 ==============================================================================
 */

#include "MIDIController.h"
#include "MIDIControllerUI.h"
#include "MIDIManager.h"
#include "DebugHelpers.h"
#include "JsHelpers.h"

extern AudioDeviceManager &  getAudioDeviceManager();

MIDIController::MIDIController() :
Controller("MIDI"),JsEnvironment("MIDI.MIDIController")
{
	setNamespaceName("MIDI."+nameParam->stringValue());
	deviceInName = addStringParameter("midiPortName", "name of Midi device input", "");
	scriptPath = addStringParameter("jsScriptPath", "path for js script", "");
	logIncoming = addBoolParameter("logIncoming","log Incoming midi message",false);

	channelFilter = addIntParameter("Channel", "Channel to filter message (0 = accept all channels)",0,0,16);

	for (int i = 0; i < 127; i++)
	{
		String noteName = MidiMessage::getMidiNoteName(i, true, true, 0);
		FloatParameter * fp = new FloatParameter(noteName, "Value for " + noteName, 0, 0, 1);
		ControlVariable * cv = addVariable(fp);
		cv->includeInSave = false;
	}

	for (int i = 0; i < 127; i++)
	{
		FloatParameter *fp = new FloatParameter(String("CC ") + String(i), String("ControlChange ") + String(i), 0, 0, 1);
		ControlVariable * cv = addVariable(fp);
		cv->includeInSave = false;
	}

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

ControllerUI * MIDIController::createUI()
{
  return new MIDIControllerUI(this);
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

      int variableIndex = 128 + message.getControllerNumber() -1;
      //DBG("Variable name " << variables[variableIndex]->parameter->niceName);
      variables[variableIndex]->parameter->setValue(message.getControllerValue()*1.f / 127.f);
    }
    else if(message.isNoteOnOrOff()){
      if (logIncoming->boolValue())
      {
        NLOG("MIDI", "Note " + String(message.isNoteOn() ? "on" : "off") + " : " + MidiMessage::getMidiNoteName(message.getNoteNumber(), true, true, 0) + " > " + String(message.getVelocity()) + " (Channel " + String(message.getChannel()) + ")");
      }
      int variableIndex = message.getNoteNumber();
      variables[variableIndex]->parameter->setValue(message.isNoteOn()?(message.getVelocity()*1.f / 127.f):0);
    }

    else if (message.isPitchWheel()){
      if (logIncoming->boolValue())
      {
        NLOG("MIDI", "pitch wheel " + String(message.getPitchWheelValue()));
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
  }
  else if(message.isNoteOnOrOff()){
    static const Identifier onCCFunctionName("onNote");
    Array<var> args;
    args.add(message.getNoteNumber());
    args.add(message.isNoteOn()?message.getVelocity():0);
    callFunctionFromIdentifier(onCCFunctionName, args);
  }
  else if(message.isPitchWheel()){
    static const Identifier onPitchWheelFunctionName("onPitchWheel");
    Array<var> args;
    args.add(message.getPitchWheelValue());
    callFunctionFromIdentifier(onPitchWheelFunctionName, args);
  }
}

void MIDIController::onContainerParameterChanged(Parameter * p){
  if(p==nameParam){
    setNamespaceName("MIDI."+nameParam->stringValue());
  }
  else if (p==deviceInName){
    setCurrentDevice(deviceInName->stringValue());
  }
  else if(p==scriptPath){
    loadFile(scriptPath->stringValue());
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



  obj.setProperty(jsPtrIdentifier, (int64)this);
  setLocalNamespace(obj);



};


void MIDIController::newJsFileLoaded(){

  scriptPath->setValue(currentFile.getRelativePathFrom(File::getCurrentWorkingDirectory()));
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
