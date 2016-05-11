/*
 ==============================================================================

 Logger.h
 Created: 6 May 2016 1:37:41pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef LGMLLOGGER_H_INCLUDED
#define LGMLLOGGER_H_INCLUDED

#include "JuceHeader.h"
#include "QueuedNotifier.h"



class LGMLLogger : public Logger{
    public :

    juce_DeclareSingleton(LGMLLogger, true);

    LGMLLogger(){
        fileLog = FileLogger::createDefaultAppLogger("LGML", "log", "");

    }
    void logMessage (const String& message) override;


    QueuedNotifier<String> notifier;
    typedef QueuedNotifier<String>::Listener Listener;
    void addLogListener(Listener * l){notifier.addListener(l);}
    void removeLogListener(Listener * l){notifier.removeListener(l);}
    
    
    ScopedPointer<FileLogger> fileLog;
    
};



#endif  // LOGGER_H_INCLUDED
