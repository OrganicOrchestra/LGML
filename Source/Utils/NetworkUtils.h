/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#pragma once

#include "../JuceHeaderCore.h"

class OSCClientRecord
{
public:
    OSCClientRecord(): port (0)
    {

    }
    OSCClientRecord (  const String& _name, IPAddress _ipAddress, String _description, uint16 _port):
        name (_name),
        ipAddress (_ipAddress),
        description (_description),
        port (_port)
    {

    }

    String name;
    IPAddress ipAddress;
    String description;
    uint16 port;

    String getShortName()
    {
        StringArray arr;
        arr.addTokens (name, ".", "");
        return arr[0];
    }
    bool isValid() {return port != 0;}
};

class NetworkUtils
{
public:
    juce_DeclareSingleton (NetworkUtils, true);

    NetworkUtils();
    ~NetworkUtils();
    static bool isValidIP (const String& ip);
    static OSCClientRecord hostnameToOSCRecord (const String& hn);

    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void oscClientAdded (OSCClientRecord ) = 0;
        virtual void oscClientRemoved (OSCClientRecord ) = 0;
    };
    ListenerList<Listener> listeners;
    void addListener (Listener* l) {listeners.add (l);}
    void removeListener (Listener* l) {listeners.remove (l);}
    Array<OSCClientRecord> getOSCRecords();

private:
    class Pimpl;
    ScopedPointer<Pimpl> pimpl;

    friend class Pimpl;
    void addOSCRecord (OSCClientRecord& o);
    void removeOSCRecord (OSCClientRecord& o);

    HashMap<String, OSCClientRecord> dnsMap;
    Array<IPAddress> localIps;

};

