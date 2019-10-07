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

//String messageToParamName(const MidiMessage & m){
//
//}
//stdMidiMessage

#include "../ControllerFactory.h"
REGISTER_CONTROLLER_TYPE (MIDIController,"MIDI");




#define NON_BLOCKING 0
template<>
void ParameterContainer::OwnedFeedbackListener<MIDIController>::parameterFeedbackUpdate (ParameterContainer* originContainer, ParameterBase* p,ParameterBase::Listener * notifier){


    if (owner->enabledParam->boolValue() && (!owner->blockFeedback->boolValue() || notifier!=(Controller*)owner))
    {
#if NON_BLOCKING
        auto f = [this,c](){
#endif
            jassert(originContainer==&owner->userContainer);
            if(owner->midiOutDevice.get() ){

                if(p){
                    auto msgToSend = MIDIHelpers::midiMessageFromParam(p, owner->channelFilter->intValue());
                    if(!msgToSend.isSysEx()){
                        owner->sendMessage(msgToSend);
                    }
                    else{
                        NLOGW(owner->getControlAddress().toString(),"user parameter doesnt have a valid midi name to be sent to device");
                    }
                }
            }
#if NON_BLOCKING
        };
        // avoid locking other threads
        if(MessageManager::getInstance()->isThisTheMessageThread()){
            f();
        }
        else{
            MessageManager::callAsync(f);
        }
#endif
    }



}


MIDIController::MIDIController (StringRef name) :
Controller (name), JsEnvironment ("controllers.MIDI", this),
midiChooser(this,true,false),
midiClock(false),
pSync(this)
{

    setNamespaceName ("controllers." + shortName);

    logIncoming = addNewParameter<BoolParameter> ("logIncoming", "log Incoming midi message", false);
    logIncoming->setSavable(false);
    logOutgoing = addNewParameter<BoolParameter> ("logOutgoing", "log Outgoing midi message", false);
    logOutgoing->setSavable(false);

    sendMIDIClock = addNewParameter<BoolParameter> ("send MIDI Clock", "send MIDI Clock",false);
    sendMIDIPosition = addNewParameter<BoolParameter> ("send MIDI Position", "send MIDI Position information",false);
    sendMIDIPosition->setEnabled(sendMIDIClock->boolValue());
    midiClockOffset = addNewParameter<IntParameter>("MIDI clock offset", "offset to apply to midiclock",0, -300,300);
    channelFilter = addNewParameter<IntParameter> ("Channel", "Channel to filter message (0 = accept all channels)", 0, 0, 16);
    midiClock.setOutput(this);
    userContainer.addFeedbackListener (&pSync);
}

MIDIController::~MIDIController()
{
    setCurrentDevice ("");

}


void MIDIController::handleIncomingMidiMessage (MidiInput*,
                                                const MidiMessage& message)
{

    if (!enabledParam->boolValue()) return;

    if (channelFilter->intValue() > 0 && message.getChannel() != channelFilter->intValue())
    {
        if (logIncoming->boolValue()){
            OLOG("MIDI IN : Ignoring MIDI message Channel filter not valid");
        }
        return;
    }
    if (logIncoming->boolValue()){
        OLOG("MIDI IN : "+MIDIHelpers::midiMessageToDebugString(message));
    }
    if (message.isController())
    {


        const String paramName(MIDIHelpers::midiMessageToParamName(message));
        if (Controllable* c = userContainer.getControllableByName(paramName))
        {
            (( ParameterBase*)c)->setValueFrom((Controller*)this,message.getControllerValue()*1.0/127);
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

        const String paramName (MIDIHelpers::midiMessageToParamName(message));//+"_"+String(message.getChannel()));
        if (Controllable* c = userContainer.getControllableByName(paramName))
        {
            (( ParameterBase*)c)->setValueFrom((Controller*)this,isNoteOn?message.getFloatVelocity():0);
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

    }
    else
    {
    }

    // call Js
    if (message.isNoteOff())
    {
        callJs (MidiMessage::noteOff (message.getChannel(), message.getNoteNumber(), 0.0f));
    }
    else
    {
        callJs (message);
    }

    if(!message.isNoteOff())inActivityTrigger->triggerDebounced(activityTriggerDebounceTime);
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

void MIDIController::onContainerParameterChanged ( ParameterBase* p)
{
    Controller::onContainerParameterChanged (p);

    if (p == nameParam)
    {
        setNamespaceName ("controllers." + shortName);
    }
    else if(p==midiChooser.getDeviceInEnumParameter()){
        auto ep = midiChooser.getDeviceInEnumParameter();
        auto selId = ep->getFirstSelectedId();
        bool connected = ep->getModel()->isValidId(selId);
        isConnected->setValue(connected);
        startMidiClockIfNeeded();

    }
    else if(p==isConnected){
        startMidiClockIfNeeded();
    }
    else if(p==sendMIDIClock){
        sendMIDIPosition->setEnabled(sendMIDIClock->boolValue());
        startMidiClockIfNeeded();
    }
    else if(p==sendMIDIPosition){
        midiClock.sendSPP = sendMIDIPosition->boolValue();
    }
    else if(p==midiClockOffset){
        midiClock.delta = midiClockOffset->intValue();
        midiClock.reset();
    }

}
void MIDIController::startMidiClockIfNeeded(){
    if(isConnected->boolValue()){
        if(sendMIDIClock->boolValue()){
            if(!midiClock.isThreadRunning())
                midiClock.start();
            else{
                midiClock.reset();
            }
        }
        else{
            midiClock.stop();
        }
    }
    else{
        midiClock.stop();
    }
}
void MIDIController::midiMessageSent(const MidiMessage & msg){
    outActivityTrigger->triggerDebounced(activityTriggerDebounceTime);
    if(logOutgoing->boolValue()){
        OLOG("MIDI OUT : "+MIDIHelpers::midiMessageToDebugString(msg));
    }

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

    
    setLocalNamespace (obj);



};




// @ben do we do the same as OSC MIDI IN/OUt controllers

var MIDIController::sendNoteOnFromJS (const var::NativeFunctionArgs& a )
{

    MIDIController* c = castPtrFromJSEnv<MIDIController> (a);

    if (a.numArguments < 3)
    {
        NLOGE(c->getControlAddress().toString(), juce::translate("Script : Incorrect number of arguments for sendNoteOn"));
        return var (false);
    }

    bool sent = c->sendNoteOn ((int) (a.arguments[0]), a.arguments[1], a.arguments[2]);
    if(!sent){
        NLOGE(c->getControlAddress().toString(),juce::translate("Script : MIDI Out is not set"));
    }
    return var (true);
}


var MIDIController::sendNoteOffFromJS (const var::NativeFunctionArgs& a)
{

    MIDIController* c = castPtrFromJSEnv<MIDIController> (a);

    if (a.numArguments < 3)
    {
        NLOGE(c->getControlAddress().toString(), juce::translate("Script : Incorrect number of arguments for sendNoteOff"));
        return var (false);
    }

   bool sent  = c->sendNoteOff ((int) (a.arguments[0]), a.arguments[1], a.arguments[2]);
    if(!sent){
        NLOGE(c->getControlAddress().toString(),juce::translate("Script : MIDI Out is not set"));
    }
    return var (sent);
}

var MIDIController::sendCCFromJS (const var::NativeFunctionArgs& a)
{
    MIDIController* c = castPtrFromJSEnv<MIDIController> (a);

    if (a.numArguments < 3)
    {
        NLOGE(c->getControlAddress().toString(), juce::translate("Script : Incorrect number of arguments for sendCC"));
        return var (false);
    }
    int targetChannel = (int) (a.arguments[0]);
    if(targetChannel==0)
        targetChannel = c->channelFilter->intValue();
    if(targetChannel==0)
        targetChannel = 1;
    bool sent =c->sendCC (targetChannel, a.arguments[1], a.arguments[2]);
    if(!sent){
        NLOGE(c->getControlAddress().toString(),juce::translate("Script : MIDI Out is not set"));
    }
    return var (sent);
}

var MIDIController::sendSysExFromJS (const var::NativeFunctionArgs& a)
{
    MIDIController* c = castPtrFromJSEnv<MIDIController> (a);

    if (a.numArguments > 8)
    {
        NLOGE(c->getControlAddress().toString(), juce::translate("Script : Incorrect number of arguments for sendSysEx"));
        return var (false);
    }

    uint8 bytes[8];

    for (int i = 0; i < a.numArguments; i++)
    {

        bytes[i] = (uint8) (int)a.arguments[i];
        LOG ("Byte : " << String (bytes[i]));
    }

    bool sent = c->sendSysEx (bytes, a.numArguments);
    if(!sent){
        NLOGE(c->getControlAddress().toString(),juce::translate("Script : MIDI Out is not set"));
    }
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
