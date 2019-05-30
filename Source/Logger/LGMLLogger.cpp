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
#include "../Engine.h"
juce_ImplementSingleton (LGMLLogger);

int LGMLLogger::maxLoggedElements = 5000;
#define CIRCULAR 0

LGMLLogger::LGMLLogger():
writeCursor(0),
welcomeMessage
(
 String("LGML v123 : (456) \n by OrganicOrchestra")
 .replace("123",String (Engine::versionString))
 .replace("456",String (Time::getCompilationDate()
                        .formatted("%d/%m/%y (%R)")))
 ){
#if CIRCULAR
    loggedElements.resize(maxLoggedElements);
    loggedElements.fill(nullptr);
#endif

#if USE_FILE_LOGGER
    addLogListener (&fileWriter);
#endif

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
    }
    else{
#if CIRCULAR
        writeCursor.set((writePos+1)%maxLoggedElements);
        auto overriden = loggedElements.getUnchecked(writePos);
        if(overriden){delete overriden;}
        loggedElements.set(writePos, el);
#else
        writeCursor.set(writePos+1);
        loggedElements.add(el);
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
/*
 void addToFifo (const int* someData, int numItems)
 {
 int start1, size1, start2, size2;
 abstractFifo.prepareToWrite (numItems, start1, size1, start2, size2);

 if (size1 > 0)
 copySomeData (myBuffer + start1, someData, size1);

 if (size2 > 0)
 copySomeData (myBuffer + start2, someData + size1, size2);

 abstractFifo.finishedWrite (size1 + size2);
 }

 void readFromFifo (int* someData, int numItems)
 {
 int start1, size1, start2, size2;
 abstractFifo.prepareToRead (numItems, start1, size1, start2, size2);

 if (size1 > 0)
 copySomeData (someData, myBuffer + start1, size1);

 if (size2 > 0)
 copySomeData (someData + size1, myBuffer + start2, size2);

 abstractFifo.finishedRead (size1 + size2);
 }
 */
