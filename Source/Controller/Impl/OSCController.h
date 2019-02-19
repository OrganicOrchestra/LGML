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


#ifndef OSCCONTROLLER_H_INCLUDED
#define OSCCONTROLLER_H_INCLUDED

#include "../Controller.h"

#include <juce_osc/juce_osc.h>


#define NUM_OSC_MSG_IN_A_ROW 100
#define OSC_QUEUE_LENGTH 5000


class OSCController :
    public Controller,
    public OSCReceiver::Listener<OSCReceiver::RealtimeCallback>
{
public:
    OSCController (const String& name);
    virtual ~OSCController();

    StringParameter* localPortParam;
    EnumParameter* remoteHostParam;
    StringParameter* remotePortParam;
    FloatParameter* speedLimit;
    BoolParameter* logIncomingOSC;
    BoolParameter* logOutGoingOSC;
    BoolParameter* blockFeedback; // if a parameter is updated from processOSC , stops any osc out with same address
    Trigger* sendAllParameters;
    


    float lastOSCMessageSentTime;


    void processMessage (const OSCMessage& msg);
    virtual Result processMessageInternal (const OSCMessage& msg);


    virtual void onContainerParameterChanged ( ParameterBase* p) override;
    virtual void onContainerTriggerTriggered (Trigger* t) override;


    void sendAllControllableStates (ControllableContainer* c, int& sentControllable );

    static StringArray OSCAddressToArray (const String&);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OSCController)


    template <typename... Args>
    bool sendOSC (const OSCAddressPattern& address, Args&& ... args)
    {
        OSCMessage m = OSCMessage (address, std::forward<Args> (args)...);
        return sendOSC (m);
    }
    bool sendOSC (OSCMessage& m);


    class OSCMessageQueue: private Timer
    {
    public:
        OSCMessageQueue (OSCController* o);
        void add (OSCMessage* m);
        void timerCallback() override;
        OwnedArray<OSCMessage > messages;
        AbstractFifo aFifo;
        OSCController* owner;
        int interval;

    };

    OSCMessageQueue oscMessageQueue;


    void logMessage (const OSCMessage& m, const String& prefix = "");

    bool setParameterFromMessage ( ParameterBase* c, const OSCMessage& msg, bool force = false,bool allowConversions = true);

    void sendOSCForAddress (const Controllable* c, const String& cAddress);
    void sendOSCFromParam(const Controllable* c);

private:

    void oscMessageReceived (const OSCMessage& message) override;
    void oscBundleReceived (const OSCBundle& bundle) override;
    
    void setupReceiver();
    void setupSender();
    bool sendOSCInternal (OSCMessage& m);
    friend class OSCMessageQueue;
    // should use sendOSC for centralizing every call
    OSCReceiver receiver;
    OSCSender sender;
    //  String lastAddressReceived;
    OSCMessage lastMessageReceived;
    bool isProcessingOSC;

    void checkAndAddParameterIfNeeded (const OSCMessage& msg);


    void resolveHostnameIfNeeded();
    bool connectSender(String & ip,int port);
    bool hostNameResolved ;

    String remoteIP;


    static HashMap<OSCController * , int> listenedPorts;
    static HashMap<OSCController * , int> localSentPorts;
    static bool AnyoneIsListeningOnPort(int);
    static bool AnyoneIsSendingOnLocalOnPort(int);
};




#endif  // OSCCONTROLLER_H_INCLUDED
