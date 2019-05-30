/*
  ==============================================================================

    Version.cpp
    Created: 9 Mar 2018 4:58:47pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "Version.h"

#if JUCE_DONT_DECLARE_PROJECTINFO
#error
#endif
#include "AppConfig.h"
#include "JuceHeader.h" // for project info


VersionTriplet::VersionTriplet()
:  major ((ProjectInfo::versionNumber & 0xff0000) >> 16),
minor ((ProjectInfo::versionNumber & 0x00ff00) >> 8),
patch ((ProjectInfo::versionNumber & 0x0000ff) >> 0)
{}

VersionTriplet::VersionTriplet (int lgmlVersionNumber)
:  major ((lgmlVersionNumber & 0xff0000) >> 16),
minor ((lgmlVersionNumber & 0x00ff00) >> 8),
patch ((lgmlVersionNumber & 0x0000ff) >> 0)
{}

VersionTriplet::VersionTriplet(int M,int m,int p):major(M),minor(m),patch(p){}

VersionTriplet::VersionTriplet(const String & M,const String &  m,const String & p){
    major = stringPartToInt(M);
    minor = stringPartToInt(m);
    patch = stringPartToInt(p);
}
VersionTriplet::VersionTriplet(const String & fullString){
    StringArray tk ;
    tk.addTokens (fullString, juce::StringRef ("."), juce::StringRef ("\""));
    jassert(tk.size()>=2);
    major = tk.size()==0?-1:stringPartToInt(tk[0]);
    minor = tk.size()<=1?-1:stringPartToInt(tk[1]);
    patch = tk.size()<=2?-1:stringPartToInt(tk[2]);
}

String VersionTriplet::toString() const {
    return intPartToString (major)+"."+intPartToString (minor) + "." + intPartToString (patch);
}


bool VersionTriplet::isCompatible(VersionTriplet & other) const{
    return partAreCompatible(major,other.major)
    && partAreCompatible(minor,other.minor)
    && partAreCompatible(patch,other.patch);
}

bool VersionTriplet::fromUnsafeString (const String& versionString,VersionTriplet& result)
{
    StringArray tokenizedString = StringArray::fromTokens (versionString, ".", StringRef());

    if (tokenizedString.size() != 3)
        return false;

    result.major = tokenizedString [0].getIntValue();
    result.minor = tokenizedString [1].getIntValue();
    result.patch = tokenizedString [2].getIntValue();

    return true;
}



bool VersionTriplet::operator> (const VersionTriplet& b) const noexcept
{
    if (major == b.major)
    {
        if (minor == b.minor)
            return patch > b.patch;

        return minor > b.minor;
    }

    return major > b.major;
}

 String VersionTriplet::intPartToString(const int p){
    if(p==-1){return "x";}
    else return String(p);
}
 int VersionTriplet::stringPartToInt(const String & s){
    if(s=="x"){return -1;};
    return s.getIntValue();
}
 bool VersionTriplet::partAreCompatible(const int a,const int b){
    if(a==-1 || b==-1){return true;}
    return a==b;
    }
