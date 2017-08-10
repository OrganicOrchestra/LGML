/*
  ==============================================================================

    NetworkUtils.h
    Created: 30 Jul 2017 3:58:07pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class OSCClientRecord{
public:
  OSCClientRecord():port(0){

  }
  OSCClientRecord(  const String& _name,IPAddress _ipAddress,String _description,uint16 _port):
  name(_name),
  ipAddress(_ipAddress),
  description(_description),
  port(_port){
    
  }
  
  String name;
  IPAddress ipAddress;
  String description;
  uint16 port;

  String getShortName(){
      StringArray arr;
      arr.addTokens(name, ".","");
      return arr[0];
  }
  bool isValid(){return port!=0;}
};

class NetworkUtils{
public:
  juce_DeclareSingleton(NetworkUtils, true);

  NetworkUtils();
  ~NetworkUtils();
  static bool isValidIP(const String & ip);
  static OSCClientRecord hostnameToOSCRecord(const String & hn);

  

private:
   class Pimpl;
  ScopedPointer<Pimpl> pimpl;
  
  HashMap<String,OSCClientRecord> dnsMap;

};

