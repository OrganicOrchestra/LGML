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
    DBG(message);
    listeners.call(&LGMLLogger::Listener::newMessage,message);
    
};
