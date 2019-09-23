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

#include "LGMLLogger.h"
#include "../Version.h"
juce_ImplementSingleton (LGMLLogger);
int LGMLLogger::maxLoggedElements = 5000;
#define CIRCULAR 0



LGMLLogger::LGMLLogger():
writeCursor(0),
welcomeMessage
(
 String("LGML v123 : 456\n789\n by OrganicOrchestra")
 .replace("123",String (VersionTriplet::getCurrentVersion().toString()))
 .replace("456",String (Time::getCompilationDate()
                        .formatted("%d/%m/%y (%R)")))
 .replace("789",BUILD_VERSION_UID)
 ){
#if CIRCULAR
    loggedElements.resize(maxLoggedElements);
    loggedElements.fill(nullptr);
#endif

#if USE_FILE_LOGGER
    addLogListener (&fileWriter);
#endif

}

LGMLLogger::~LGMLLogger(){
    
}

const String & LGMLLogger::getWelcomeMessage(){
    return welcomeMessage;
}


void LGMLLogger::logMessage (const String& message)
{


    // get last to check for doublons
    int writePos = writeCursor.get();
    LogElement * lastLogged(nullptr);
#if CIRCULAR

    int lastIdx =(writePos+maxLoggedElements - 1)%maxLoggedElements;
    lastLogged =  loggedElements.getUnchecked(lastIdx);
#else
    if(loggedElements.size()>0){lastLogged=loggedElements.getUnchecked(loggedElements.size()-1);}

#endif

    LogElement * el = new LogElement(message);
    if(lastLogged && *lastLogged==*el){
        delete el;
        lastLogged->incrementNumAppearances();
        el = lastLogged;
        writePos = jmax(0,loggedElements.size()-1);
    }
    else{
#if CIRCULAR
        writeCursor.set((writePos+1)%maxLoggedElements);
        auto overriden = loggedElements.getUnchecked(writePos);
        if(overriden){delete overriden;}
        loggedElements.set(writePos, el);
#else
        writePos = loggedElements.size();
        loggedElements.add(el);
        writeCursor .set( writePos);


#endif
    }
    listeners.call (&Listener::newMessageAtIdx, writePos);

    DBG (message);

}

int LGMLLogger::getNumLogs(){
    #if CIRCULAR
    if(loggedElements.getUnchecked(loggedElements.size()-1)==nullptr){
        return writeCursor.get();
    }
    #endif
    return loggedElements.size();
}

void LGMLLogger::clearLog(){
    writeCursor.set(0);
    loggedElements.clearQuick(true);
    listeners.call (&Listener::newMessageAtIdx, -1);
}



LogElement::LogElement(const String& log):
source (DebugHelpers::getLogSource (log)),
content (DebugHelpers::getLogContent (log)),
numAppearances(1)
,_arr (new StringArray())
{

    time = Time::getCurrentTime();
    _arr->addTokens (content, StringRef ("\r\n"), StringRef("\""));

    if (_arr->size())
    {
        String* s = &_arr->getReference (0);
        auto cp = s->getCharPointer();
        severity = LOG_NONE;

        while (cp.getAndAdvance() == '!' && severity < LOG_ERR)
        {
            severity = (Severity) (severity + 1);
        }

        if (severity == LOG_NONE && s->startsWith ("JUCE Assertion"))
        {
            severity = LOG_ERR;
        }
        else
        {
            if(severity!=LOG_NONE)
                _arr->set (0, _arr->getReference (0).substring ((int)severity + 2));
        }

    }
    else
    {
        severity = LOG_NONE;
    }

}
