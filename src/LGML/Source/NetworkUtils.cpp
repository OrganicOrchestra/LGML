/*
 ==============================================================================

 NetworkUtils.cpp
 Created: 30 Jul 2017 3:58:07pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "NetworkUtils.h"
#include "DebugHelpers.h"

// TODO implement dns support on linux / windows
#define SUPPORT_DNS JUCE_MAC

juce_ImplementSingleton(NetworkUtils);




NetworkUtils::~NetworkUtils(){

}

bool NetworkUtils::isValidIP(const String & ip){
  StringArray arr;
  arr.addTokens(ip,".","\"");
  return arr.size()==4;
}
void NetworkUtils::addOSCRecord(OSCClientRecord & oscRec){
  auto key = oscRec.getShortName();
  dnsMap.set(key , oscRec);
  DBG("found OSC : " << oscRec.getShortName() << " ("<<oscRec.name << ")");
  MessageManager::getInstance()->callAsync([this,&oscRec](){listeners.call(&Listener::oscClientAdded,oscRec);});
}
void NetworkUtils::removeOSCRecord(OSCClientRecord & oscRec){
  dnsMap.remove(oscRec.getShortName());
  MessageManager::getInstance()->callAsync([this,&oscRec](){listeners.call(&Listener::oscClientRemoved,oscRec);});
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
  Pimpl():Thread("bonjourOSC"){
    startBrowse();



  }
  ~Pimpl(){
    stopThread(-1);
  }

  void startBrowse(){
    // scan all interfaces
    std::unordered_set<uint32_t> idxs;
    {
      struct ifaddrs *ifap = NULL;
      if(getifaddrs(&ifap) < 0) {LOG("Cannot not get a list of interfaces\n");return;}
      for(struct ifaddrs *p = ifap; p!=NULL; p=p->ifa_next) {

        idxs.insert( if_nametoindex(p->ifa_name));
      }
      freeifaddrs(ifap);
    }

    for(auto i:idxs){
      browse("_osc._udp","local",i);
//      browse("_aftovertcp._udp","",i);
//      browse("_aftovertcp._tcp","",i);
    }
    
  }
  std::unordered_map<DNSServiceRef,int> m_ClientToFdMap;

  void run() override{
    bool shouldStop = false;
    while(isThreadRunning() && !shouldStop){
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
      wait(200);
    }
    DBG("dns thread ended");
  }
  void browse(const std::string& regType, const std::string& domain,uint32_t itf_idx){
    if(auto * nu = NetworkUtils::getInstanceWithoutCreating()){
      nu->dnsMap.clear();
    }
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
    String host (hosttarget);
    {
      // format hostname to ip part
      if(host.endsWith(".local.")){host=host.substring(0,host.length()-7);}
      StringArray arr;
      arr.addTokens(host,"-","");
      while(arr.size()>4){
        arr.remove(arr.size()-1);
      }
      if(arr.size()==4){
        host = arr.joinIntoString(".");
      }
    }
    IPAddress ip(host);
    DBG(ip.toString());
    String description = String::fromUTF8((char*)txtRecord);
    uint16 host_port = ntohs(port);
    OSCClientRecord oscRec{name,ip,description,host_port};

    nu->addOSCRecord(oscRec);

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

};
OSCClientRecord NetworkUtils::hostnameToOSCRecord(const String & hn)
//int hostname_to_ip(char * hostname , char* ip)
{
  LOG("ip hostname discovery not supported on windows/Unix");
  return OSCClientRecord();
}

#endif



NetworkUtils::NetworkUtils(){
  pimpl = new Pimpl;
  
}
