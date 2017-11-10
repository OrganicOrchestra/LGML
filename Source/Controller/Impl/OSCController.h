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


#define NUM_OSC_MSG_IN_A_ROW 10
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
    
    BoolParameter* isConnectedToRemote;



    float lastOSCMessageSentTime;
    int numSentInARow;


    void processMessage (const OSCMessage& msg);
    virtual Result processMessageInternal (const OSCMessage& msg);


    virtual void onContainerParameterChanged (Parameter* p) override;
    virtual void onContainerTriggerTriggered (Trigger* t) override;

    virtual void oscMessageReceived (const OSCMessage& message) override;
    virtual void oscBundleReceived (const OSCBundle& bundle) override;
    void sendAllControllableStates (ControllableContainer* c, int& sentControllable );

    static StringArray OSCAddressToArray (const String&);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OSCController)

    //Listener
    class  OSCControllerListener
    {
    public:
        /** Destructor. */
        virtual ~OSCControllerListener() {}
        virtual void messageProcessed (const OSCMessage& msg, bool success) = 0;
    };

    ListenerList<OSCControllerListener> oscListeners;
    void addOSCControllerListener (OSCControllerListener* newListener) { oscListeners.add (newListener); }
    void removeOSCControllerListener (OSCControllerListener* listener) { oscListeners.remove (listener); }

#if JUCE_COMPILER_SUPPORTS_VARIADIC_TEMPLATES
    template <typename... Args>
    bool sendOSC (const OSCAddressPattern& address, Args&& ... args)
    {
        OSCMessage m = OSCMessage (address, std::forward<Args> (args)...);
        return sendOSC (m);
    }
#endif

    class OSCMessageQueue: private Timer
    {
    public:
        OSCMessageQueue (OSCController* o);
        void add (OSCMessage* m);
        void timerCallback() override;
        Array<OSCMessage*> messages;
        AbstractFifo aFifo;
        OSCController* owner;
        int interval;

    };

    OSCMessageQueue oscMessageQueue;
    bool sendOSC (OSCMessage& m);

    void logMessage (const OSCMessage& m, const String& prefix = "");

    bool setParameterFromMessage (Parameter* c, const OSCMessage& msg, bool force = false,bool allowConversions = true);

private:

    
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
    bool hostNameResolved ;

    String remoteIP;

};




#endif  // OSCCONTROLLER_H_INCLUDED
