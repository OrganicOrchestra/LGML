/*
  ==============================================================================

    QueuedNotifier.h
    Created: 11 May 2016 4:29:20pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef QUEUEDNOTIFIER_H_INCLUDED
#define QUEUEDNOTIFIER_H_INCLUDED



template<typename ArrayClass,class CriticalSectionToUse = CriticalSection>
class QueuedNotifier:public  AsyncUpdater{
public:

    QueuedNotifier():getOnlyLastValue(false){}
    virtual ~QueuedNotifier() {}

    // allow to stack all values or get oly last updated value
    bool getOnlyLastValue;


    class Listener{
    public:
        virtual ~Listener(){}
        virtual void newMessage(const ArrayClass&) = 0;
    };



    void addMessage( ArrayClass * msg,bool forceSendNow = false){
        if(listeners.size()==0){
            delete msg;
            return;
        }
        forceSendNow |= MessageManager::getInstance()->isThisTheMessageThread();
        if(forceSendNow){
            listeners.call(&Listener::newMessage,*msg);
            delete msg;
        }
        else{
            if(getOnlyLastValue){
                if(messageQueue.size()<1){messageQueue.add(msg);}
                else{messageQueue.set(0, msg);}
            }
            else{messageQueue.add(msg);}

            triggerAsyncUpdate();
        }

    }
    void addListener(Listener* newListener) { listeners.add(newListener); }
    void removeListener(Listener* listener) { listeners.remove(listener); }
private:

    void handleAsyncUpdate() override
    {

        {
            const typename CriticalSectionToUse::ScopedLockType lk(messageQueue.getLock());
            for(auto &v:messageQueue){
                listeners.call(&Listener::newMessage,*v);
            }
        }
        messageQueue.clear();

    }

    OwnedArray<ArrayClass,CriticalSectionToUse> messageQueue;
    
    ListenerList<Listener > listeners;
    
};




#endif  // QUEUEDNOTIFIER_H_INCLUDED
