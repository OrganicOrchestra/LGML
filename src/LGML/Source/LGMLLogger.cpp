/*
  ==============================================================================

    Logger.cpp
    Created: 6 May 2016 1:37:41pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "LGMLLogger.h"

juce_ImplementSingleton(LGMLLogger);

void LGMLLogger::logMessage (const String& message) {
    listeners.call(&LGMLLogger::Listener::newMessage,message);
    if(fileLog){
        fileLog->logMessage(message);
    }
    
};
