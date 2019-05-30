/*
  ==============================================================================

    VersionTriplet.h
    Created: 30 May 2019 8:47:13pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once


class VersionTriplet{
public:

    VersionTriplet (int lgmlVersionNumber);
    VersionTriplet(int M,int m,int p);
    VersionTriplet(const String & M,const String &  m,const String & p);
    VersionTriplet(const String & fullString);

    String toString() const ;
    static bool fromUnsafeString (const String& versionString,VersionTriplet& result);

    bool isCompatible(VersionTriplet & other) const;
    bool operator> (const VersionTriplet& b) const noexcept;
    VersionTriplet getMasked( int level)const;
    static VersionTriplet getCurrentVersion();
    static const char* getProductName();
    
private:

    static String intPartToString(const int p);
    static int stringPartToInt(const String & s);
    static bool partAreCompatible(const int a,const int b);
    int major;
    int minor;
    int patch;
};
