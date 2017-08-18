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


#include "NetworkUtils.h"
#include "DebugHelpers.h"

// TODO implement dns support on  windows
#ifndef SUPPORT_DNS
  #if (JUCE_MAC || JUCE_LINUX)
    #define SUPPORT_DNS 1
  #else
    #define SUPPORT_DNS 0
  #endif
#endif // SUPPORT_DNS

juce_ImplementSingleton(NetworkUtils);




NetworkUtils::~NetworkUtils(){

}

bool NetworkUtils::isValidIP(const String & ip){
  StringArray arr;
  arr.addTokens(ip,".","\"");
  return arr.size()==4;
}
void NetworkUtils::addOSCRecord(OSCClientRecord & oscRec){
//  if(!localIps.contains(oscRec.ipAddress)){
    auto key = oscRec.getShortName();
    dnsMap.set(key , oscRec);
    DBG("found OSC : " << oscRec.getShortName() << " ("<<oscRec.name << ")");

  MessageManager::getInstance()->callAsync([this,oscRec](){listeners.call(&Listener::oscClientAdded,oscRec);});
//  }
}
void NetworkUtils::removeOSCRecord(OSCClientRecord & oscRec){
  dnsMap.remove(oscRec.getShortName());
  MessageManager::getInstance()->callAsync([this,oscRec](){listeners.call(&Listener::oscClientRemoved,oscRec);});
}


#if SUPPORT_DNS

#include <dns_sd.h>
#include <net/if.h>     // For if_nametoindex()

#include <netdb.h> //hostent

#include <arpa/inet.h>
#include <ifaddrs.h>


#include <unordered_map>
#include <unordered_set>





class NetworkUtils::Pimpl: private Thread{
public:
  Pimpl(NetworkUtils *_nu):Thread("bonjourOSC") ,nu(_nu){
    name = getName();
    // scan all interfaces starting with en
    struct ifaddrs *ifap = NULL;
    if(getifaddrs(&ifap) < 0) {LOG("Cannot not get a list of interfaces\n");return;}
    for(struct ifaddrs *p = ifap; p!=NULL; p=p->ifa_next) {
      if( String(p->ifa_name).startsWith("en")){
        if_idxs.insert( if_nametoindex(p->ifa_name));
      }
    }
    freeifaddrs(ifap);
    

    register_OSC();
    startBrowse();



  }
  ~Pimpl(){
    stopThread(-1);
    for ( auto ii = m_ClientToFdMap.cbegin() ; ii != m_ClientToFdMap.cend() ; ) {
      auto jj = ii++;
      DNSServiceRefDeallocate(jj->first);
    }
    for ( auto ii = m_ServerToFdMap.cbegin() ; ii != m_ServerToFdMap.cend() ; ) {
      auto jj = ii++;
      DNSServiceRefDeallocate(jj->first);
    }
  }

  std::unordered_set<uint32_t> if_idxs;
  std::unordered_map<DNSServiceRef,int> m_ClientToFdMap;
  std::unordered_map<DNSServiceRef,int> m_ServerToFdMap;
  String name;
  NetworkUtils * nu;
  static constexpr int defaultOSCPort = 11000;
  

  String getName(){
    return SystemStats::getComputerName();

  }
  void register_OSC(){
    for(auto i : if_idxs){
      DNSServiceRef client(0);
      if(DNSServiceRegister(
                            &client,
                            0,   //DNSServiceFlags flags,
                            i,//uint32_t interfaceIndex,
                            ("LGML_"+name).toRawUTF8(),//const char                          *name,         /* may be NULL */
                            "_osc._udp",//const char                          *regtype,
                            "local",//const char                          *domain,       /* may be NULL */
                            NULL,//const char                          *host,         /* may be NULL */
                            htons(defaultOSCPort),//uint16_t port,                                     /* In network byte order */
                            0,//uint16_t txtLen,
                            NULL,//const void                          *txtRecord,    /* may be NULL */
                            NULL,//DNSServiceRegisterReply callBack,                  /* may be NULL */
                            NULL//void                                *context       /* may be NULL */
                            )==0){
        m_ServerToFdMap[client] = DNSServiceRefSockFD(client);
      }
    }
  }
  void startBrowse(){

      nu->dnsMap.clear();
      OSCClientRecord rec = {"localhost",IPAddress(String("127.0.0.1")),"local computer" ,defaultOSCPort};
      nu->addOSCRecord(rec);


    for(auto i:if_idxs){
      browse("_osc._udp","local",i);
      //      browse("_afptovertcp._udp","",i);
      //      browse("_aftovertcp._tcp","",i);
    }

  }



  void run() override{
    bool shouldStop = false;
    while(!threadShouldExit()  && !shouldStop){
      int count = 0;
      for ( ; ; ) {
        if ( m_ClientToFdMap.size() == 0 ) {
          DBG("dns ended");
          shouldStop = true;
          break;
        }
        fd_set readfds;
        FD_ZERO(&readfds);
        int ndfs=0;
        if(m_ClientToFdMap.size()){
          ndfs = m_ClientToFdMap.begin()->second + (m_ClientToFdMap.size()-1);
        }
        for ( auto ii = m_ClientToFdMap.cbegin() ; ii != m_ClientToFdMap.cend() ; ii++ ) {
          FD_SET(ii->second, &readfds);
        }

        struct timeval tv = { 0, 1000 };
        int result = select(ndfs, &readfds, (fd_set*)NULL, (fd_set*)NULL, &tv);
        if ( result > 0 ) {
          //
          // While iterating through the loop, the callback functions might delete
          // the client pointed to by the current iterator, so I have to increment
          // it BEFORE calling DNSServiceProcessResult
          //
          for ( auto ii = m_ClientToFdMap.cbegin() ; ii != m_ClientToFdMap.cend() ; ) {
            auto jj = ii++;
            if (FD_ISSET(jj->second, &readfds) ) {
              if((DNSServiceProcessResult(jj->first)!=0)) jassertfalse;
              if ( ++count > 10 )
                break;
            }
          }
        } else
          break;
        if ( count > 10 )
          break;
      }
      sleep(200);
    }
    DBG("dns thread ended");
  }
  void browse(const std::string& regType, const std::string& domain,uint32_t itf_idx){

    DNSServiceRef client(0);

    DNSServiceErrorType err = DNSServiceBrowse(&client, 0, itf_idx, regType.c_str(), domain.empty() ? 0 : domain.c_str(), cb_dns, this);
    if(err!=kDNSServiceErr_NoError){
      DBG("can't browse : err = " <<err);
    }
    else{
      m_ClientToFdMap[client] = DNSServiceRefSockFD(client);
      startThread();
    }
  }

  static void cb_resolve(
                         DNSServiceRef sdRef,
                         DNSServiceFlags flags,
                         uint32_t interfaceIndex,
                         DNSServiceErrorType errorCode,
                         const char                          *fullname,
                         const char                          *hosttarget,
                         uint16_t port,                                   /* In network byte order */
                         uint16_t txtLen,
                         const unsigned char                 *txtRecord,
                         void                                *context
                         ){

    auto * nu = (NetworkUtils*) context;
    String name(fullname);
    String hostIP (hosttarget);
    {
      // format hostname to ip part
      if(hostIP.endsWith(".local.")){hostIP=hostIP.substring(0,hostIP.length()-7);}
      StringArray arr;
      arr.addTokens(hostIP,"-","");
      while(arr.size()>4){
        arr.remove(arr.size()-1);
      }
      if(arr.size()==4){
        hostIP = arr.joinIntoString(".");
      }
      else{
#if 0
        // deprecated gethostbyname
        struct hostent * he = gethostbyname(hosttarget);
        if( he->h_length){
          struct in_addr **addr_list = (struct in_addr **)he->h_addr_list;
          hostIP = inet_ntoa(*addr_list[0]);
        }

        else{
          DBG("DNS : can't resolve non ip name");
          jassertfalse;
        }
        freehostent(he);
#else
        struct addrinfo hints,*res=NULL;
        memset(&hints,0,sizeof(hints));
        hints.ai_family = AF_INET;

        if(getaddrinfo(hosttarget,NULL,&hints,&res)==0){

          struct sockaddr_in *addr;
          struct addrinfo * it = res;
          while(it){
            if(it->ai_family==AF_INET){
              addr = (struct sockaddr_in *)it->ai_addr;
              hostIP = inet_ntoa((struct in_addr)addr->sin_addr);
              break;
            }
            it = it->ai_next;
          }

        }
        if(res)
        freeaddrinfo(res);

#endif
      }
    }
    if(isValidIP(hostIP)){

    IPAddress ip(hostIP);
    DBG(ip.toString());
    String description = String::fromUTF8((char*)txtRecord);
    uint16 host_port = ntohs(port);
    OSCClientRecord oscRec{name,ip,description,host_port};

    nu->addOSCRecord(oscRec);

    }
    else{
      jassertfalse;
      LOG("DNS : can't resolve ip :"<< hostIP << " (" <<String(hosttarget)<<")" );
    }

  }
  static void cb_dns(
                     DNSServiceRef sdRef,
                     DNSServiceFlags flags,
                     uint32_t interfaceIndex,
                     DNSServiceErrorType errorCode,
                     const char                          *serviceName,
                     const char                          *regtype,
                     const char                          *replyDomain,
                     void                                *context
                     ){


    DNSServiceRef client(0);
    if(DNSServiceResolve(&client,0,interfaceIndex,serviceName,regtype,replyDomain,cb_resolve, NetworkUtils::getInstance())==0){
      auto * pimpl = (Pimpl*)context;
      pimpl->m_ClientToFdMap[client] = DNSServiceRefSockFD(client);
    }
    else{
      jassertfalse;
    }


  };



};


OSCClientRecord  NetworkUtils::hostnameToOSCRecord(const String & hn)
{
  auto * nu = NetworkUtils::getInstanceWithoutCreating();
  if(nu){
    // will return emty if not in there
    return nu->dnsMap[hn];
    
  }
  return OSCClientRecord();
}
#else


// dummy implementation
class NetworkUtils::Pimpl{
public:
  Pimpl(NetworkUtils*){}
  
};
OSCClientRecord NetworkUtils::hostnameToOSCRecord(const String & )
//int hostname_to_ip(char * hostname , char* ip)
{
  LOG("ip hostname discovery not supported on windows/Unix");
  return OSCClientRecord();
}

#endif



NetworkUtils::NetworkUtils(){
  IPAddress::findAllAddresses(localIps,false);
  pimpl = new Pimpl(this);
  
}
