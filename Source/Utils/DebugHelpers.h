/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#ifndef DEBUGHELPERS_H_INCLUDED
#define DEBUGHELPERS_H_INCLUDED
//  header (build and link-time cheap) to include everywhere we need LGML specific debug macros

static const String logSplitter("::");
// slower but safe log (avoid flooding
#define SLOG(textToWrite) JUCE_BLOCK_WITH_FORCED_SEMICOLON (juce::String tempDbgBuf;\
static uint32 lastTime =  Time::getMillisecondCounter(); \
static bool runningUnderDebugger = juce_isRunningUnderDebugger();\
uint32 now = Time::getMillisecondCounter();\
if( (now - lastTime>300 )|| runningUnderDebugger){ \
String fullPath = String(__FILE__);\
tempDbgBuf << fullPath.substring (fullPath.lastIndexOfChar (File::getSeparatorChar()) + 1 ,fullPath.lastIndexOfChar('.') ) << logSplitter <<  textToWrite;\
juce::Logger::writeToLog(tempDbgBuf);\
lastTime = now;})


// log informing file from where it was outputed
#define LOG(textToWrite) JUCE_BLOCK_WITH_FORCED_SEMICOLON (juce::String tempDbgBuf;\
String fullPath = String(__FILE__);\
tempDbgBuf << fullPath.substring (fullPath.lastIndexOfChar (File::getSeparatorChar()) + 1 ,fullPath.lastIndexOfChar('.') ) << logSplitter <<  textToWrite;\
juce::Logger::writeToLog(tempDbgBuf);)

#define LOGW(t) LOG("!! " << t)
#define LOGE(t) LOG("!!! " << t)
// named version where source name is user defined

#define NLOG(__name,textToWrite) JUCE_BLOCK_WITH_FORCED_SEMICOLON (juce::String tempDbgBuf;\
tempDbgBuf << __name << logSplitter << textToWrite;\
juce::Logger::writeToLog(tempDbgBuf);)

#define NLOGW(n,t) NLOG(n,"!! " << t)
#define NLOGE(n,t) NLOG(n,"!!! " << t)

namespace DebugHelpers{
inline void debugDobj(const var & v){
    DBG(JSON::toString(v));
}


inline String getLogSource (const String& logString)
{
    return logString.substring (0, logString.indexOf (logSplitter)).trim();
}

inline String getLogContent (const String& logString)
{
    int startString = logString.indexOf (logSplitter);

    if (startString >= 0)startString += 2;
    else startString = 0;

    return logString.substring ( startString, logString.length()).trim();
}

}



#endif  // DEBUGHELPERS_H_INCLUDED
