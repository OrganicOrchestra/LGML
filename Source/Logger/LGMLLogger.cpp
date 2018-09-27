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

LGMLLogger::LGMLLogger():
notifier (5000),
welcomeMessage
(
 String("LGML v123 : (456) \n by OrganicOrchestra")
 .replace("123",String (Engine::versionString))
 .replace("456",String (Time::getCompilationDate()
                        .formatted("%d/%m/%y (%R)")))
 ){

#if USE_FILE_LOGGER
        addLogListener (&fileWriter);
#endif

}

const String & LGMLLogger::getWelcomeMessage(){
    return welcomeMessage;
}


void LGMLLogger::logMessage (const String& message)
{
    notifier.addMessage (new String (message));
    DBG (message);

}
