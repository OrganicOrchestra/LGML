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

AudioDeviceManager & getAudioDeviceManager();

MIDIController::MIDIController() :
Controller("MIDI"),JsEnvironment("MIDI.MIDIController")
{
    setNamespaceName("MIDI."+nameParam->stringValue());
}

ControllerUI * MIDIController::createUI()
{
    return new MIDIControllerUI(this);
}

void MIDIController::handleIncomingMidiMessage (MidiInput* source,
                                                const MidiMessage& message)
{

	if (!enabledParam->boolValue()) return;

    if(message.isController()){
        LOG("Incoming controlChange message : " + String(source->getName()) + " / " + String(message.getControllerValue()));
    }
    else if(message.isNoteOnOrOff()){
        LOG("Incoming note message : " + String(source->getName()) + " / " + String(message.getNoteNumber()) + " / "
            + (message.isNoteOn()?"on":"off"));
    }

    callJs(message);
    
    
}

void MIDIController::callJs(const MidiMessage& message){
    if(message.isController()){
        static const Identifier onCCFunctionName("onCC");
        Array<var> args;
        args.add(message.getControllerNumber());
        args.add(message.getControllerValue());
        callFunctionFromIdentifier(onCCFunctionName, args);
    }
    if(message.isNoteOnOrOff()){
        static const Identifier onCCFunctionName("onNote");
        Array<var> args;
        args.add(message.getNoteNumber());
        args.add(message.isNoteOn()?message.getVelocity():0);
        callFunctionFromIdentifier(onCCFunctionName, args);
    }
}

void MIDIController::onContainerParameterChanged(Parameter * p){
    if(p==nameParam){
        setNamespaceName("MIDI."+nameParam->stringValue());
    }
}

void MIDIController::buildLocalEnv(){
    DynamicObject obj;
    obj.setMethod("sendCC", sendCC);
    obj.setProperty(ptrIdentifier, this);
    obj.setMethod("sendNoteOnFor", sendNoteOnFor);
    setLocalNamespace(obj);
    
};


// @ben do we do the same as OSC MIDI IN/OUt controllers

var MIDIController::sendCC(const var::NativeFunctionArgs & a){
//    MIDIController * c = getObjectPtrFromJS<MIDIController>(a);



    return var::undefined();
}
var MIDIController::sendNoteOnFor(const var::NativeFunctionArgs & a){

        return var::undefined();
}