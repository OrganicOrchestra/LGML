/*
  ==============================================================================

    DebugHelpers.h
    Created: 6 May 2016 2:10:12pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef DEBUGHELPERS_H_INCLUDED
#define DEBUGHELPERS_H_INCLUDED
//  header (build and link-time cheap) to include everywhere we need LGML specific debug macros

#include "JuceHeader.h"


// log informing file from where it was outputed
#define LOG(textToWrite) JUCE_BLOCK_WITH_FORCED_SEMICOLON (juce::String tempDbgBuf;\
String fullPath = String(__FILE__);\
tempDbgBuf << fullPath.substring (fullPath.lastIndexOfChar (File::separator) + 1 ,fullPath.lastIndexOfChar('.') ) << " : " <<  textToWrite;\
juce::Logger::writeToLog(tempDbgBuf);)


// named version where source name is user defined
#define NLOG(__name,textToWrite) JUCE_BLOCK_WITH_FORCED_SEMICOLON (juce::String tempDbgBuf;\
tempDbgBuf << __name << " : " << textToWrite;\
juce::Logger::writeToLog(tempDbgBuf);)



inline String getLogSource(const String & logString) {
    return logString.substring(0, logString.indexOf(":")).trim();
}

inline String getLogContent(const String & logString) {
    return logString.substring( logString.indexOf(":")+1,logString.length()).trim();
}

class LogElement{
public:
    LogElement(const String & log):source(getLogSource(log)),content(getLogContent(log)){
        _arr.addTokens(content,"\n","");
    }
    String content;
    String source;
    int getNumLines(){return  _arr.size();}
    const String & getLine(int i){return _arr[i]; }


private:
        StringArray _arr;

};



#endif  // DEBUGHELPERS_H_INCLUDED
