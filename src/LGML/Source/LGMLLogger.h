/*
 ==============================================================================

 Logger.h
 Created: 6 May 2016 1:37:41pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef LGMLLOGGER_H_INCLUDED
#define LGMLLOGGER_H_INCLUDED

#include "QueuedNotifier.h"

// do not use file logger atm
// TODO figure out true utility of such
#define USE_FILE_LOGGER 0




class LGMLLogger : public Logger {
    public :

    juce_DeclareSingleton(LGMLLogger, true);

    LGMLLogger():notifier(100){
#if USE_FILE_LOGGER
        addLogListener(&fileWriter);
#endif
    }

	void logMessage(const String & message) override;


    QueuedNotifier<String> notifier;
    typedef QueuedNotifier<String>::Listener Listener;



    void addLogListener(Listener * l){notifier.addListener(l);}
    void removeLogListener(Listener * l){notifier.removeListener(l);}

#if USE_FILE_LOGGER
    class FileWriter : public Listener{
    public:
        FileWriter(){fileLog = FileLogger::createDefaultAppLogger("LGML", "log", "");}

        void newMessage(const String& s) override{if (fileLog && !s.isEmpty()) {fileLog->logMessage(s);}}
        String getFilePath(){return fileLog->getLogFile().getFullPathName();}
        ScopedPointer<FileLogger> fileLog;
    };

    FileWriter fileWriter;
#endif
};



#endif  // LOGGER_H_INCLUDED
