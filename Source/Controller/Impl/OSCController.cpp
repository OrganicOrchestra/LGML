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


#include "OSCController.h"

#include "../../Utils/DebugHelpers.h"

#include "../../Node/Manager/NodeManager.h"

#include "../../Utils/NetworkUtils.h"




class OSCClientModel: public EnumParameterModel, NetworkUtils::Listener
{

public:

    OSCClientModel(): EnumParameterModel()
    {
        auto nu = NetworkUtils::getInstance();
        nu->addListener (this);

        for (auto r : nu->getOSCRecords())
        {
            oscClientAdded (r);
        }
    };
    ~OSCClientModel()
    {
    }

    void oscClientAdded (OSCClientRecord o)
    {
        addOrSetOption (o.getShortName(), o.ipAddress.toString(), true);
    };
    void oscClientRemoved (OSCClientRecord o)
    {
        removeOption (o.getShortName(), true);
    };

};

int defaultLocalOSCPort = 11000;
OSCController::OSCController (const String& _name) :
    Controller (_name),
    lastMessageReceived (OSCAddressPattern ("/fake")),
    isProcessingOSC (false),
    oscMessageQueue (this),
    hostNameResolved (false)

{
    // force init of Network Utils if not created
    NetworkUtils::getInstance();

    localPortParam = addNewParameter<StringParameter> ("Local Port", "The port to bind for the controller to receive OSC from it", String(defaultLocalOSCPort++));
    remotePortParam = addNewParameter<StringParameter> ("Remote Port", "The port bound by the controller to send OSC to it", "8000");
    static OSCClientModel model;
    remoteHostParam = addNewParameter<EnumParameter> ("Remote Host", "The host's IP of the remote controller", &model, var ("localhost"), true);

    
    logIncomingOSC = addNewParameter<BoolParameter> ("logIncomingOSC", "log the incoming OSC Messages", false);
    logOutGoingOSC = addNewParameter<BoolParameter> ("logOutGoingOSC", "log the outGoing OSC Messages", false);
    speedLimit = addNewParameter<FloatParameter> ("speedLimit", "min interval (ms) between 2 series of " + String (NUM_OSC_MSG_IN_A_ROW) + " OSCMessages", 0.f, 0.f, 100.f);

    blockFeedback = addNewParameter<BoolParameter> ("blockFeedback", "block osc feedback (resending updated message to controller)", true);
    sendAllParameters =  addNewParameter<Trigger> ("sendAll", "send all parameter states to initialize ", true);

    
    setupReceiver();
    setupSender();

    receiver.addListener (this);
    lastOSCMessageSentTime = 0;




}

OSCController::~OSCController()
{
    
}

void OSCController::setupReceiver()
{
    // DBG("setupReceiver");
    receiver.disconnect();

    if (!receiver.connect (localPortParam->stringValue().getIntValue()))
    {

        LOGE(juce::translate("can't connect to local port : ") + localPortParam->stringValue());
    };

    //DBG("Receiver connected" + String(result));
}
void OSCController::setupSender()
{
    isConnected->setValue (false);
    sender.disconnect();
    hostNameResolved = false;
    resolveHostnameIfNeeded();

    if (!hostNameResolved)
    {
        LOGE(juce::translate("no valid ip found for ") << remoteHostParam->stringValue());
    }

}


void OSCController::resolveHostnameIfNeeded()
{
    if (hostNameResolved) return;

    String hostName = remoteHostParam->stringValue();

    if (hostName.isNotEmpty())
    {
        if (!NetworkUtils::isValidIP (hostName))
        {
            OSCClientRecord resolved = NetworkUtils::hostnameToOSCRecord (hostName);

            if (resolved.isValid())
            {
                remoteIP = resolved.ipAddress.toString();
                if(resolved.hasValidPort()){
                String resolvedPortString = String ((int)resolved.port);

                if (!remotePortParam->isSettingValue() && remotePortParam->stringValue() != resolvedPortString)
                {

                    //    ensure to not create feedback on ports
                    if(remoteIP == "127.0.0.1" &&
                       resolved.port == localPortParam->stringValue().getIntValue()){
                        resolvedPortString = String((int)resolved.port+1);

                    }

                    //          call again with resolved port if not manually set
                    remotePortParam->setValue (resolvedPortString, false, true);
                    hostNameResolved = true;
                    return;
                }
                }
                hostNameResolved = true;
                int portNum = remotePortParam->stringValue().getTrailingIntValue();
                bool connected = portNum>=100 && portNum <= 20000;
                if(connected)connected = sender.connect (remoteIP,portNum );
                isConnected->setValue (connected);

                if(connected){
                    LOG (juce::translate("resolved IP : ") << hostName << " > " << remoteIP << ":" << remotePortParam->stringValue());
                }
                else{
                    LOGW (juce::translate("unresolved IP : ") << hostName << " > " << remoteIP << ":" << remotePortParam->stringValue());
                }

            }
            else
            {
                LOGW(juce::translate("can't resolve IP : ") << hostName);
            }
        }
        else
        {
            remoteIP = hostName;
            int portNum  = remotePortParam->stringValue().getTrailingIntValue();
            isConnected->setValue ((portNum >= 100 && portNum <= 20000)?
                                           sender.connect (remoteIP, portNum):
                                           false);
            hostNameResolved = true;
        }
    }

}
void OSCController::processMessage (const OSCMessage& msg)
{
    bool isPing =msg.getAddressPattern()=="/ping";
    if(isPing){
        sendOSC("/pong",getControlAddress());
        inActivityTrigger->triggerDebounced(activityTriggerDebounceTime); // only show led activity on pings
        return;
    }


    bool logIncoming = logIncomingOSC->boolValue();
    if ( logIncoming)
    {
        logMessage (msg, "In:");

    }

    if (!enabledParam->boolValue()) return;

    if (blockFeedback->boolValue())
    {
        lastMessageReceived = msg;
    }

    isProcessingOSC = true;




    Result result = isPing?Result::ok():processMessageInternal (msg);
    

    if (autoAddParams && !result && !msg.getAddressPattern().containsWildcards())
    {
        MessageManager::getInstance()->callAsync ([this, msg]() {checkAndAddParameterIfNeeded (msg);});
    }
    if(logIncoming && !result){
        LOGW(result.getErrorMessage());
    }
    isProcessingOSC = false;
    

    inActivityTrigger->triggerDebounced(activityTriggerDebounceTime);
}


bool OSCController::setParameterFromMessage ( ParameterBase* c, const OSCMessage& msg, bool force,bool allowConversions)
{
    auto  targetType = c->getFactoryTypeId();

    if (targetType == ParameterProxy::_factoryType) targetType = ((ParameterProxy*)c)->linkedParam->getFactoryTypeId();


    if (targetType == Trigger::_factoryType)
    {
        if (msg.size() == 0) ((Trigger*)c)->trigger();
        else if (allowConversions && (msg[0].isInt32() || msg[0].isFloat32()))
        {
            float val = msg[0].isInt32() ? msg[0].getInt32() : msg[0].getFloat32();

            if (val > 0) ((Trigger*)c)->trigger();
        }
    }
    else if (targetType == BoolParameter::_factoryType)
    {
        if (msg.size() > 0 && (
            (allowConversions || (msg[0].isInt32() && (msg[0].getInt32()==0 || msg[0].getInt32()==1))) &&
            (msg[0].isInt32() || msg[0].isFloat32())))
        {
            float val = msg[0].isInt32() ? msg[0].getInt32() : msg[0].getFloat32();
            (( ParameterBase*)c)->setValue (val > 0, false, force);
        }
    }
    else if (targetType == FloatParameter::_factoryType)
    {
        if (msg.size() > 0 && ((allowConversions && msg[0].isInt32()) || msg[0].isFloat32()))
        {
            float value = msg[0].isInt32() ? msg[0].getInt32() : msg[0].getFloat32();
            (( ParameterBase*)c)->setValue ((float)value, false, force); //normalized or not ? can user decide ?
        }
    }
    else if (targetType == IntParameter::_factoryType)
    {
        if (msg.size() > 0 && (msg[0].isInt32() || (allowConversions && msg[0].isFloat32())))
        {
            int value = msg[0].isInt32() ? msg[0].getInt32() : (int)msg[0].getFloat32();
            (( ParameterBase*)c)->setValue (value, false, force);
        }
    }
    else if (targetType == StringParameter::_factoryType)
    {
        if (msg.size() > 0)
        {
            // cast number to strings
            if  (allowConversions && (msg[0].isInt32() || msg[0].isFloat32()))
            {
                float value = msg[0].isInt32() ? msg[0].getInt32() : msg[0].getFloat32();
                (( ParameterBase*)c)->setValue (String (value));
            }
            else if (msg[0].isString())
            {
                (( ParameterBase*)c)->setValue (msg[0].getString(), false, force);
            }
        }
    }
    else if (targetType == EnumParameter::_factoryType)
    {
        if (msg.size() > 0)
        {
            // cast float to int
            if  (allowConversions && (msg[0].isInt32() || msg[0].isFloat32()))
            {
                int value = msg[0].isInt32() ? msg[0].getInt32() : msg[0].getFloat32();
                (( ParameterBase*)c)->setValue (value, false, force);
            }
            // select by name
            else if (msg[0].isString())
            {
                (( ParameterBase*)c)->setValue (msg[0].getString(), false, force);
            }
        }
    }
    else
    {
        return false;
    }



    return true;
}


void OSCController::checkAndAddParameterIfNeeded (const OSCMessage& msg)
{

    // cannot create a param from a wildcard address
    if(msg.getAddressPattern().containsWildcards())
        return;
    // TODO handle wildcards
    String addr = msg.getAddressPattern().toString();
    auto* linked = ParameterBase::fromControllable (userContainer.getControllableForAddress (addr));

    if (!linked)
    {

        StringArray sa = OSCAddressToArray (addr);
        ParameterContainer* tC = &userContainer;

        for ( int i = 0 ; i < sa.size() - 1 ; i++)
        {
            auto* c = dynamic_cast<ParameterContainer*> (tC->getControllableContainerByName (sa[i], true));

            if (!c)
            {
                c = new ParameterContainer (sa[i]);
                c->setUserDefined (true);
                tC->addChildControllableContainer (c, true);
            }

            tC = c ? c : nullptr;
        }

        String pName = sa[sa.size() - 1];

        if (tC)
        {
            if (msg.size() == 0)
            {
                linked = tC->addNewParameter<Trigger> (pName, "entry for " + msg.getAddressPattern().toString());
            }
            else
            {
                if (msg[0].isString())
                {
                    linked = tC->addNewParameter<StringParameter> (pName, "entry for " + msg.getAddressPattern().toString());
                }
                else if (msg[0].isInt32())
                {
                    linked = tC->addNewParameter<IntParameter> (pName, "entry for " + msg.getAddressPattern().toString());
                }
                else if (msg[0].isFloat32())
                {
                    linked = tC->addNewParameter<FloatParameter> (pName, "entry for " + msg.getAddressPattern().toString());
                }
            }

            if (linked)
            {
                setParameterFromMessage (linked, msg, true);
            }
        }
        else
        {
            jassertfalse;
        }

    }




}

void OSCController::logMessage (const OSCMessage& msg, const String& prefix)
{
    String log(prefix + msg.getAddressPattern().toString() + ":");

    for (int i = 0 ; i < msg.size() ; i++)
    {
        OSCArgument a = msg[i];

        if (a.isInt32())log += String (msg[i].getInt32()) + " ";
        else if (a.isFloat32())log += String (msg[i].getFloat32(), 2) + " ";
        else if (a.isString())log += String (msg[i].getString()) + " ";

    }

    NLOG (getNiceName(), log);
}

Result OSCController::processMessageInternal (const OSCMessage&)
{
    return Result::fail ("Not handled"); //if not overriden, msg is not handled so result is false
}

void OSCController::onContainerParameterChanged ( ParameterBase* p)
{
    Controller::onContainerParameterChanged (p);

    if (p == localPortParam) setupReceiver();
    else if ((p == remotePortParam && !remoteHostParam->isSettingValue()) || p == remoteHostParam ) setupSender();
    else if (p == speedLimit) {oscMessageQueue.interval = speedLimit->floatValue();}



}

void OSCController::onContainerTriggerTriggered (Trigger* t)
{
    Controller::onContainerTriggerTriggered (t);

    if (t == sendAllParameters)
    {
        int sentCount = 0;
        sendAllControllableStates (NodeManager::getInstance(), sentCount);
    }
}

void OSCController::oscMessageReceived (const OSCMessage& message)
{
    //DBG("Message received !");
    processMessage (message);

}

void OSCController::oscBundleReceived (const OSCBundle& bundle)
{
    for (auto& m : bundle)
    {
        processMessage (m.getMessage());
    }
}


inline bool compareOSCArg (const OSCArgument& a, const OSCArgument& b)
{
    // if floats and ints : compare their values independently of their types
    if  (a.getType() == OSCTypes::float32 && b.getType() == OSCTypes::int32)
    {
        return a.getFloat32() == b.getInt32();
    }

    if(a.getType() == OSCTypes::int32 && b.getType() == OSCTypes::float32){
        return a.getInt32() == b.getFloat32();
    }

    if (a.getType() != b.getType())
    {
        return false;
    }

    if (a.getType() == OSCTypes::float32)
    {
        return a.getFloat32() == b.getFloat32();
    }

    if (a.getType() == OSCTypes::string)
    {
        return a.getString() == b.getString();
    }

    if (a.getType() == OSCTypes::int32)
    {
        return a.getInt32() == b.getInt32();
    }

    if (a.getType() == OSCTypes::blob)
    {
        return a.getBlob() == b.getBlob();
    }

    return false;
}

inline bool compareOSCMessages (const  OSCMessage& a, const OSCMessage& b)
{
    if (a.getAddressPattern() != b.getAddressPattern())
    {
        return false;
    }

    if (a.size() != b.size())
    {
        return false;
    }

    for (int i = 0 ; i < a.size(); i++)
    {
        if (!compareOSCArg (a[i], b[i]))
        {
            return false;
        }
    }

    return true;

}
bool OSCController::sendOSC (OSCMessage& m)
{
    if (enabledParam->boolValue() )
    {
        resolveHostnameIfNeeded();

        if (hostNameResolved)
        {
            if (!blockFeedback->boolValue() ||   !compareOSCMessages (lastMessageReceived, m)) //!isProcessingOSC ||
            {

                if (speedLimit->floatValue() > 0.0f)
                {
                    oscMessageQueue.add (new OSCMessage (m));
                }
                else
                {
                    return sendOSCInternal (m);
                }

            }
        }
    }

    return false;
}
bool OSCController::sendOSCInternal (OSCMessage& m)
{
    if (logOutGoingOSC->boolValue()) { logMessage (m, "Out:");}
    outActivityTrigger->triggerDebounced(activityTriggerDebounceTime);
    if(isConnected->boolValue()){
    return sender.send (m);
    }
    else{
        LOG(String("OSC : 123 not connected").replace("123",nameParam->stringValue()));

    }
    return false;
}



void OSCController::sendAllControllableStates (ControllableContainer* c, int& sentControllable)
{
    if (c)
    {
        for (auto& controllable : c->getAllControllables(false))
        {
            sendOSCFromParam(controllable);
            sentControllable++;

            if ((sentControllable % 10) == 0)
            {
                Thread::sleep (2);
            }
        }

        for (auto& container : c->controllableContainers)
        {
            sendAllControllableStates (container, sentControllable);
        }
    }

}

void OSCController::sendOSCFromParam(const Controllable *c){
    if (c->isChildOf (&userContainer))
    {
        sendOSCForAddress (c, c->getControlAddress (&userContainer));
    }
    else
    {
        sendOSCForAddress (c, c->controlAddress);
    }
}

void OSCController::sendOSCForAddress (const Controllable* c, const String& cAddress)
{


    if (const ParameterBase* p = ParameterBase::fromControllable (c))
    {
        auto  targetType = p->getFactoryTypeId();

        if (targetType == ParameterProxy::_factoryType) targetType = ((ParameterProxy*)c)->linkedParam->getFactoryTypeId();

        if (targetType == Trigger::_factoryType) {sendOSC (cAddress);}
        else if (targetType == BoolParameter::_factoryType) {sendOSC (cAddress, p->intValue());}
        else if (targetType == FloatParameter::_factoryType) {sendOSC (cAddress, p->floatValue());}
        else if (targetType == IntParameter::_factoryType) {sendOSC (cAddress, p->intValue());}
        else if (targetType == StringParameter::_factoryType) {sendOSC (cAddress, p->stringValue());}
        else if (targetType == EnumParameter::_factoryType) {sendOSC (cAddress, p->stringValue());}
        else if (targetType == Point2DParameter<int>::_factoryType) {
            auto point = static_cast<const Point2DParameter<int> *>(p);
            sendOSC (cAddress, point->getX(),point->getY());
        }
        else if (targetType == Point2DParameter<floatParamType>::_factoryType) {
            auto point = static_cast<const Point2DParameter<floatParamType> *>(p);
            sendOSC (cAddress, (float)point->getX(),(float)point->getY());
        }
        else
        {
            DBG ("Type not supported " << targetType.toString());
            jassertfalse;
        }

    }
    else
    {
        jassertfalse;
    }
}


////////////////////////
// OSCMessageQueue
///////////////////////
OSCController::OSCMessageQueue::OSCMessageQueue (OSCController* o):
    owner (o),
    aFifo (OSC_QUEUE_LENGTH),
    interval (1)
{
//    messages.resize (OSC_QUEUE_LENGTH);
}

void OSCController::OSCMessageQueue::add (OSCMessage* m)
{
    int startIndex1, blockSize1, startIndex2, blockSize2;
    aFifo.prepareToWrite (1, startIndex1, blockSize1, startIndex2, blockSize2);
    int numWritten = 0;

    // fifo is full : we can drop message
    while (blockSize1 == 0)
    {
        aFifo.finishedRead (1);
        aFifo.prepareToWrite (1, startIndex1, blockSize1, startIndex2, blockSize2);
        NLOGE (owner->getNiceName(), juce::translate("still flooding OSC"));
    }
    if (blockSize1 > 0)
    {
        if(messages.size()<OSC_QUEUE_LENGTH){
            jassert(startIndex1 == messages.size());
            messages.add(m);
        }
        else
            messages.set (startIndex1, m);
        numWritten ++;
    }
    else if (blockSize2 > 0)
    {
        jassertfalse;
        messages.set (startIndex2, m);
        numWritten ++;
    }
    else
    {
        jassertfalse;

    }

    aFifo.finishedWrite (numWritten);

    if (!isTimerRunning())startTimer (interval);
}

void OSCController::OSCMessageQueue::timerCallback()
{
    if (aFifo.getNumReady())
    {
        int numRead = 0;
        int startIndex1, blockSize1, startIndex2, blockSize2;
        aFifo.prepareToRead (NUM_OSC_MSG_IN_A_ROW, startIndex1, blockSize1, startIndex2, blockSize2);

        if (blockSize1 > 0)
        {
            for ( ; numRead < blockSize1 ; numRead++ )
            {
                owner->sendOSCInternal (*messages[startIndex1 + numRead]);
            }
        }

        if (blockSize2 > 0)
        {
            for (int i = 0 ; i < blockSize2 ; i++ )
            {
                owner->sendOSCInternal (*messages[startIndex2 + i]);
                numRead++;
            }
        }

        aFifo.finishedRead (numRead);

    }
    else
    {
        stopTimer();
    }
}

StringArray OSCController::OSCAddressToArray (const String& addr)
{
    StringArray addrArray;
    addrArray.addTokens (addr, juce::StringRef ("/"), juce::StringRef ("\""));
    addrArray.remove (0);
    return addrArray;
}
