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


MIDIController::MIDIController() :
Controller("MIDI"),JsEnvironment("MIDI.MIDIController")
{
    setNamespaceName("MIDI."+nameParam->stringValue());
    deviceInName = addStringParameter("midiPortName", "name of Midi device input", "");
    scriptPath = addStringParameter("jsScriptPath", "path for js script", "");
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
    else if (p==deviceInName){
        setCurrentDevice(deviceInName->stringValue());
    }
    else if(p==scriptPath){
        loadFile(scriptPath->stringValue());
    }
}

void MIDIController::buildLocalEnv(){
    DynamicObject obj;
    static const Identifier jsSendCCIdentifier("sendCC");
    obj.setMethod(jsSendCCIdentifier, sendCC);
    obj.setProperty(jsPtrIdentifier, (int64)this);

    static const Identifier jsSendNoteOnForIdentifier("sendNoteOnFor");
    obj.setMethod(jsSendNoteOnForIdentifier, sendNoteOnFor);
    setLocalNamespace(obj);

};


void MIDIController::newJsFileLoaded(){
    scriptPath->setValue(currentFile.getFullPathName());
};

// @ben do we do the same as OSC MIDI IN/OUt controllers

var MIDIController::sendCC(const var::NativeFunctionArgs &){
//    MIDIController * c = getObjectPtrFromJS<MIDIController>(a);



    return var::undefined();
}
var MIDIController::sendNoteOnFor(const var::NativeFunctionArgs &){

        return var::undefined();
}
