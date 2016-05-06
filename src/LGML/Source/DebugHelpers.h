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

#define DBGLOG(textToWrite) JUCE_BLOCK_WITH_FORCED_SEMICOLON (juce::String tempDbgBuf;tempDbgBuf << typeid(*this).name();tempDbgBuf << " : " ; tempDbgBuf << textToWrite; juce::Logger::writeToLog(tempDbgBuf);)

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
