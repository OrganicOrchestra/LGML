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

#ifndef LGMLLOGGER_H_INCLUDED
#define LGMLLOGGER_H_INCLUDED


#include "../Utils/DebugHelpers.h"
#include "../Utils/QueuedNotifier.h"


#if JUCE_DEBUG || LGML_BETA
#define USE_FILE_LOGGER 1
class FileWriter;
#else
#define USE_FILE_LOGGER 0
#endif


class LogElement
{
public:
    LogElement (const String& log);
    Time time;
    String content;
    String source;
    enum Severity {LOG_NONE = -1, LOG_DBG = 0, LOG_WARN = 1, LOG_ERR = 2};
    Severity severity;
    int getNumLines() const {return  _arr->size();}
    const String& getLine (int i) const {return _arr->getReference (i); }
    void incrementNumAppearances(){numAppearances++; time=Time::getCurrentTime();}
    int getNumAppearances() const{return numAppearances;}
    bool operator == (const LogElement & other) const{
        return (other.severity == severity) &&
        (other.source == source) &&
        (other.content ==content);
    }
    String toNiceString(bool includeSeverity)const;


private:
    int numAppearances;
    std::unique_ptr<StringArray> _arr;
//    friend class LinkedListPointer<LogElement>;
//    LogElement * nextItem;
};



class LGMLLogger : public Logger
{
    public :

    juce_DeclareSingleton (LGMLLogger, true)

    LGMLLogger();
    ~LGMLLogger();

    void logMessage (const String& message) override;

    struct Listener : private AsyncUpdater{
        Listener():readIdx(0),lastIdx(0){}
        virtual ~Listener(){cancelPendingUpdate();}

        virtual void newMessage(const LogElement * el)  = 0;
        virtual void logCleared() {}
    private:
        void handleAsyncUpdate() final{
            auto &elems = LGMLLogger::getInstance()->loggedElements;
            int end = lastIdx.get()+1;
            if(end<readIdx){logCleared();readIdx = 0;}
            else if(readIdx==end && end==elems.size() ){
                auto el = elems.getUnchecked(end-1);
                auto curNumAp = el->getNumAppearances();
                if(lastNumAp!=curNumAp){
                    newMessage(el);
                    lastNumAp = curNumAp;
                }
            }
            else{
                lastNumAp  = 1;
            }
            const int maxMsgBeforeFlood = 2000;
            if(end-readIdx > maxMsgBeforeFlood){
                readIdx = jmax(0,end-50);
                elems.set(readIdx,new LogElement("!!! too many message to log dumping messages"));
            }
            for(int i = readIdx ; i < end ; i++){
                newMessage(elems.getUnchecked(i));
            }
            readIdx=jmax(0,end);
            
        }
        friend class LGMLLogger;
        void newMessageAtIdx(int idx){
            lastIdx.set(idx);
            triggerAsyncUpdate();
        }

        int readIdx;
        int lastNumAp = 0;
        Atomic<int> lastIdx;

    };

    ListenerList<Listener> listeners;


    class CoalescedListener : private Listener,private Timer{
    public:
        CoalescedListener(int _time):time(_time),readIdxCoal(0){

        }
        virtual ~CoalescedListener(){stopTimer();}
        virtual void newMessages(int from, int to)  = 0;

    private:
        void newMessage(const LogElement * ){
            if(!isTimerRunning()){
                startTimer(time);
            }
        }

        void timerCallback() final{
            int tend = LGMLLogger::getInstance()->getNumLogs();
            if(readIdxCoal> tend){ readIdxCoal = 0;}// has been cleared
            else if(tend>0 && readIdxCoal==tend)readIdxCoal=tend-1; // if only updating last
            newMessages(readIdxCoal,tend);
            readIdxCoal = tend;
            // check if any modification occured during this callback
            if(tend == LGMLLogger::getInstance()->getNumLogs()){stopTimer();}
        }
        const int time;
        int readIdxCoal;
        friend class LGMLLogger;

    };

    const String  getWelcomeMessage();
    void addLogListener (Listener* l) {listeners.add (l);}
    void addLogCoalescedListener (CoalescedListener* l) {listeners.add (l);}
    void removeLogListener (CoalescedListener* l) {listeners.remove (l);}
    void removeLogListener (Listener* l) {listeners.remove (l);}

#if USE_FILE_LOGGER
    std::unique_ptr<FileWriter> fileWriter;
#endif

    bool copyToCrashLogFile() noexcept;

    static int maxLoggedElements;
    Atomic<int> writeCursor;
    OwnedArray<LogElement,CriticalSection> loggedElements;
    int getNumLogs();
    void clearLog();

};



#endif  // LOGGER_H_INCLUDED
