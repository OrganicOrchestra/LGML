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



// TODO : create a parrallel file Log (with fileLogger)

class LGMLLogger : public Logger{
    public :

    juce_DeclareSingleton(LGMLLogger, true);

    LGMLLogger(){
        fileLog = FileLogger::createDefaultAppLogger("LGML", "log", "");

    }
    void logMessage (const String& message) override;


    //Listener
    class  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}

        virtual void newMessage(const String &)=0;
    };

    ListenerList<Listener> listeners;
    void addLogListener(Listener* newListener) { listeners.add(newListener); }
    void removeLogListener(Listener* listener) { listeners.remove(listener); }

    ScopedPointer<FileLogger> fileLog;

};



#endif  // LOGGER_H_INCLUDED
