/*
  ==============================================================================

    Logger.cpp
    Created: 6 May 2016 1:37:41pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "LGMLLogger.h"

juce_ImplementSingleton(LGMLLogger);



void LGMLLogger::logMessage(const String & message)
{
	notifier.addMessage(new String(message));
  DBG(message);

}
