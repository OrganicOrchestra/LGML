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

    // default to async listener
    class Listener : public AsyncUpdater{
    public:
        Listener(){}
        virtual ~Listener() {}

        virtual void newMessage(const String&) = 0;




    private:
        void handleAsyncUpdate()override{
            const ScopedLock lk(mu);
            for(auto &v:newMessagesToBeSent){newMessage(v);}
            newMessagesToBeSent.clear();
        }

        void notifyNextMessage(const String & s){
            {
                const ScopedLock lk(mu);
                newMessagesToBeSent.add(s);
            }

            triggerAsyncUpdate();
        };
        friend class LGMLLogger;

        StringArray newMessagesToBeSent;
        CriticalSection mu;
        
    };


    ListenerList<Listener> listeners;
    void addLogListener(Listener* newListener) { listeners.add(newListener); }
    void removeLogListener(Listener* listener) { listeners.remove(listener); }

    ScopedPointer<FileLogger> fileLog;

};



#endif  // LOGGER_H_INCLUDED
