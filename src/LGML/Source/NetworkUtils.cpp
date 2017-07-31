/*
  ==============================================================================

    NetworkUtils.cpp
    Created: 30 Jul 2017 3:58:07pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "NetworkUtils.h"




juce_ImplementSingleton(NetworkUtils);


NetworkUtils::NetworkUtils(){

  Array<juce::IPAddress> results;
  IPAddress::findAllAddresses(results);
  for (auto &p:results){
    DBG(" addr :: " << p.toString());
  }

}

NetworkUtils::~NetworkUtils(){

}


auto d = NetworkUtils::getInstance();


bool NetworkUtils::isValidIP(const String & ip){
   StringArray arr;
   arr.addTokens(ip,".","\"");
   return arr.size()==4;
}

#if JUCE_MAC || JUCE_LINUX
#include <sys/socket.h>
#include <errno.h> //For errno - the error number
#include <netdb.h> //hostent
#include <arpa/inet.h>

IPAddress NetworkUtils::hostnameToIP(const String & hn)
//int hostname_to_ip(char * hostname , char* ip)
{
  struct hostent *he;
  struct in_addr **addr_list;

  if ( (he = gethostbyname( hn.getCharPointer() ) ) == NULL)
  {
    // get the host info
    herror("gethostbyname");
    return IPAddress();
  }

  addr_list = (struct in_addr **) he->h_addr_list;

  if(addr_list[0]!=nullptr)
  {
    //Return the first one;
//    strcpy(ip , inet_ntoa(*addr_list[i]) );
    return IPAddress(String(inet_ntoa(*addr_list[0])));
  }

  return IPAddress();
}
#else
IPAddress NetworkUtils::hostnameToIP(const String & hn)
//int hostname_to_ip(char * hostname , char* ip)
{
  LOG("hostname not supported on windows");
  return IPAddress();
}

#endif
