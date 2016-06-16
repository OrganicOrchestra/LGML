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



class LGMLLogger : public Logger {
    public :

    juce_DeclareSingleton(LGMLLogger, true);

    LGMLLogger():notifier(100){
        addLogListener(&fileWriter);
    }

	void logMessage(const String & message) override;


    QueuedNotifier<String> notifier;
    typedef QueuedNotifier<String>::Listener Listener;



    void addLogListener(Listener * l){notifier.addListener(l);}
    void removeLogListener(Listener * l){notifier.removeListener(l);}

    class FileWriter : public Listener{
    public:
        FileWriter(){fileLog = FileLogger::createDefaultAppLogger("LGML", "log", "");}

        void newMessage(const String& s) override{if (fileLog) {fileLog->logMessage(s);}}
        String getFilePath(){return fileLog->getLogFile().getFullPathName();}
        ScopedPointer<FileLogger> fileLog;
    };

    FileWriter fileWriter;

};



#endif  // LOGGER_H_INCLUDED
