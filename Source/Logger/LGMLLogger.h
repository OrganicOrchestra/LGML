/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#ifndef LGMLLOGGER_H_INCLUDED
#define LGMLLOGGER_H_INCLUDED

#include "../Utils/QueuedNotifier.h"

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
