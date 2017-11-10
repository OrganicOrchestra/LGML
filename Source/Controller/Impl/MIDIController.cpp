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


#include "MIDIController.h"
#include "../../MIDI/MIDIManager.h"
#include "../../MIDI/MIDIHelpers.h"
#include "../../Utils/DebugHelpers.h"
#include "../../Scripting/Js/JsHelpers.h"

extern AudioDeviceManager&   getAudioDeviceManager();

#include "../ControllerFactory.h"
REGISTER_CONTROLLER_TYPE (MIDIController);

MIDIController::MIDIController (StringRef name) :
    Controller (name), JsEnvironment ("controllers.MIDI", this),
midiChooser(this,true,false)
{
    setNamespaceName ("controllers." + shortName);

    logIncoming = addNewParameter<BoolParameter> ("logIncoming", "log Incoming midi message", false);
    logIncoming->isSavable = false;
    logIncoming->isPresettable =false;

    channelFilter = addNewParameter<IntParameter> ("Channel", "Channel to filter message (0 = accept all channels)", 0, 0, 16);

}

MIDIController::~MIDIController()
{
    setCurrentDevice (String::empty);

}


void MIDIController::handleIncomingMidiMessage (MidiInput*,
                                                const MidiMessage& message)
{

    if (!enabledParam->boolValue()) return;

    if (channelFilter->intValue() > 0 && message.getChannel() != channelFilter->intValue())
    {
        return;
    }

    if (message.isController())
    {
        if (logIncoming->boolValue())
        {
            NLOG ("MIDI", "CC " + String (message.getControllerNumber()) + " > " + String (message.getControllerValue()) + " (Channel " + String (message.getChannel()) + ")");
        }


        const String paramName( "CC "+String(message.getControllerNumber()));
        if (Controllable* c = userContainer.getControllableByName(paramName))
        {
                ((Parameter*)c)->setValue(message.getControllerValue()*1.0/127);
        }
        else if(autoAddParams){
            MessageManager::callAsync([this,message,paramName](){
                userContainer.
                addNewParameter<FloatParameter>(
                                                paramName,
                                                "MIDI CC Parameter",
                                                message.getControllerValue()/127.0,
                                                0,1);
            }
                                      );
        }

    }
    else if (message.isNoteOnOrOff())
    {
        bool isNoteOn = message.isNoteOn();
        if (logIncoming->boolValue())
        {
            NLOG ("MIDI", "Note " + String (isNoteOn ? "on" : "off") + " : " + MidiMessage::getMidiNoteName (message.getNoteNumber(), true, true, 0) + " > " + String (message.getVelocity()) + " (Channel " + String (message.getChannel()) + ")");
        }

        const String paramName (MidiMessage::getMidiNoteName (message.getNoteNumber(), true, true, 0));//+"_"+String(message.getChannel()));
        if (Controllable* c = userContainer.getControllableByName(paramName))
        {
            ((Parameter*)c)->setValue(isNoteOn?message.getFloatVelocity():0);
        }
        else if(autoAddParams && isNoteOn ){
            MessageManager::callAsync([this,message,paramName](){
                userContainer.
                addNewParameter<FloatParameter>(paramName,
                                                "MIDI Note Parameter",
                                                message.getFloatVelocity(),
                                                0,1);
            }
                                      );
        }


    }

    else if (message.isPitchWheel())
    {
        if (logIncoming->boolValue())
        {
            NLOG ("MIDI", "pitch wheel " + String (message.getPitchWheelValue()));
        }
    }
    else
    {
        if (logIncoming->boolValue())
        {
            NLOG ("MIDI", "message : " + message.getDescription());
        }
    }

    if (message.isNoteOff())
    {
        callJs (MidiMessage::noteOff (message.getChannel(), message.getNoteNumber(), 0.0f));
    }
    else
    {
        callJs (message);
    }

    if(!message.isNoteOff())inActivityTrigger->trigger();
}




void MIDIController::callJs (const MidiMessage& message)
{
    if (message.isController())
    {
        static const Identifier onCCFunctionName ("onCC");
        Array<var> args;
        args.add (message.getControllerNumber());
        args.add (message.getControllerValue());
        callFunctionFromIdentifier (onCCFunctionName, args);

        for (auto& p : jsCCListeners)
        {
            p->processMessage (message);
        }
    }
    else if (message.isNoteOnOrOff())
    {
        static const Identifier onCCFunctionName ("onNote");
        Array<var> args;
        args.add (message.getNoteNumber());
        args.add (message.isNoteOn() ? message.getVelocity() : 0);
        callFunctionFromIdentifier (onCCFunctionName, args);

        for (auto& p : jsNoteListeners)
        {
            p->processMessage (message);
        }
    }
    else if (message.isPitchWheel())
    {
        static const Identifier onPitchWheelFunctionName ("onPitchWheel");
        Array<var> args;
        args.add (message.getPitchWheelValue());
        callFunctionFromIdentifier (onPitchWheelFunctionName, args);
    }
}

void MIDIController::onContainerParameterChanged (Parameter* p)
{
    Controller::onContainerParameterChanged (p);

    if (p == nameParam)
    {
        setNamespaceName ("controllers." + shortName);
    }
    
}

void MIDIController::midiMessageSent(){
    outActivityTrigger->trigger();

};

void MIDIController::buildLocalEnv()
{
    DynamicObject obj;

    static const Identifier jsSendNoteOnIdentifier ("sendNoteOn");
    obj.setMethod (jsSendNoteOnIdentifier, sendNoteOnFromJS);

    static const Identifier jsSendNoteOffIdentifier ("sendNoteOff");
    obj.setMethod (jsSendNoteOffIdentifier, sendNoteOffFromJS);

    static const Identifier jsSendCCIdentifier ("sendCC");
    obj.setMethod (jsSendCCIdentifier, sendCCFromJS);

    static const Identifier jsSendSysExIdentifier ("sendSysEx");
    obj.setMethod (jsSendSysExIdentifier, sendSysExFromJS);

    static const Identifier jsGetCCListenerObject ("createCCListener");
    obj.setMethod (jsGetCCListenerObject, &MIDIController::createJsCCListener);

    static const Identifier jsGetNoteListenerObject ("createNoteListener");
    obj.setMethod (jsGetNoteListenerObject, &MIDIController::createJsNoteListener);

    obj.setProperty (jsPtrIdentifier, (int64)this);
    setLocalNamespace (obj);



};


void MIDIController::newJsFileLoaded()
{


};

// @ben do we do the same as OSC MIDI IN/OUt controllers

var MIDIController::sendNoteOnFromJS (const var::NativeFunctionArgs& a )
{

    MIDIController* c = getObjectPtrFromJS<MIDIController> (a);

    if (a.numArguments < 3)
    {
        NLOG ("MidiController", "!!! Incorrect number of arguments for sendNoteOn");
        return var (false);
    }

    c->sendNoteOn ((int) (a.arguments[0]), a.arguments[1], a.arguments[2]);
    return var (true);
}


var MIDIController::sendNoteOffFromJS (const var::NativeFunctionArgs& a)
{

    MIDIController* c = getObjectPtrFromJS<MIDIController> (a);

    if (a.numArguments < 3)
    {
        NLOG ("MidiController", "!!! Incorrect number of arguments for sendNoteOff");
        return var (false);
    }

    c->sendNoteOff ((int) (a.arguments[0]), a.arguments[1], a.arguments[2]);
    return var (true);
}

var MIDIController::sendCCFromJS (const var::NativeFunctionArgs& a)
{
    MIDIController* c = getObjectPtrFromJS<MIDIController> (a);

    if (a.numArguments < 3)
    {
        NLOG ("MidiController", "!!! Incorrect number of arguments for sendCC");
        return var (false);
    }
    int targetChannel = (int) (a.arguments[0]);
    if(targetChannel==0)
        targetChannel = c->channelFilter->intValue();
    if(targetChannel==0)
        targetChannel = 1;
    c->sendCC (targetChannel, a.arguments[1], a.arguments[2]);
    return var (true);
}

var MIDIController::sendSysExFromJS (const var::NativeFunctionArgs& a)
{
    MIDIController* c = getObjectPtrFromJS<MIDIController> (a);

    if (a.numArguments > 8)
    {
        NLOG ("MidiController", "!!! Incorrect number of arguments for sendSysEx");
        return var (false);
    }

    uint8 bytes[8];

    for (int i = 0; i < a.numArguments; i++)
    {

        bytes[i] = (uint8) (int)a.arguments[i];
        LOG ("Byte : " << String (bytes[i]));
    }

    c->sendSysEx (bytes, a.numArguments);
    return var (true);
}


var MIDIController::createJsNoteListener (const var::NativeFunctionArgs& a)
{
    if (a.numArguments < 2) { return var::undefined();}

    int channel = a.arguments[0];
    int numberToListen = a.arguments[1];
    MIDIController* originEnv = dynamic_cast<MIDIController*> (a.thisObject.getDynamicObject());

    if (originEnv)
    {
        JsMIDIMessageListener* ob = new JsMIDIMessageListener (originEnv, channel, numberToListen, true);
        originEnv->jsNoteListeners.add (ob);
        return ob->object;
    }

    return var::undefined();
}
var MIDIController::createJsCCListener (const var::NativeFunctionArgs& a)
{
    if (a.numArguments < 2) { return var::undefined();}

    int channel = a.arguments[0];
    int numberToListen = a.arguments[1];
    MIDIController* originEnv = dynamic_cast<MIDIController*> (a.thisObject.getDynamicObject());

    if (originEnv)
    {
        JsMIDIMessageListener* ob = new JsMIDIMessageListener (originEnv, channel, numberToListen, false);
        originEnv->jsCCListeners.add (ob);
        return ob->object;
    }

    return var::undefined();
}

void MIDIController::clearNamespace()
{
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


Identifier JsMIDIMessageListener::midiReceivedId ("onMidi");
Identifier JsMIDIMessageListener::midiValueId ("value");
