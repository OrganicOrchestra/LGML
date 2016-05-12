/*
  ==============================================================================

    QueuedNotifier.h
    Created: 11 May 2016 4:29:20pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef QUEUEDNOTIFIER_H_INCLUDED
#define QUEUEDNOTIFIER_H_INCLUDED



template<typename MessageClass,class CriticalSectionToUse = CriticalSection>
class QueuedNotifier:public  AsyncUpdater{
public:

    QueuedNotifier(){}
    virtual ~QueuedNotifier() {}



    class Listener{
    public:
        virtual ~Listener(){}
        virtual void newMessage(const MessageClass&) = 0;
    };



    void addMessage( MessageClass * msg,bool forceSendNow = false){
        if(listeners.size()==0 && lastListeners.size()==0){
            delete msg;
            return;
        }
        forceSendNow |= MessageManager::getInstance()->isThisTheMessageThread();
        if(forceSendNow){
            listeners.call(&Listener::newMessage,*msg);
            lastListeners.call(&Listener::newMessage,*msg);
            delete msg;
        }
        else{
            if(listeners.size()==0){
                if(messageQueue.size()<1){messageQueue.add(msg);}
                else{messageQueue.set(0, msg);}
            }
            else{messageQueue.add(msg);}

            triggerAsyncUpdate();
        }

    }

    // allow to stack all values or get oly last updated value
    void addListener(Listener* newListener) { listeners.add(newListener); }
    void addAsyncCoalescedListener(Listener* newListener) { lastListeners.add(newListener); }
    void removeListener(Listener* listener) { listeners.remove(listener);lastListeners.remove(listener); }
private:

    void handleAsyncUpdate() override
    {
        
        {
            const typename CriticalSectionToUse::ScopedLockType lk(messageQueue.getLock());
            for(auto &v:messageQueue){
                listeners.call(&Listener::newMessage,*v);
            }
            lastListeners.call(&Listener::newMessage,*messageQueue.getLast());
        }
        messageQueue.clear();

    }

    OwnedArray<MessageClass,CriticalSectionToUse> messageQueue;
    
    ListenerList<Listener > listeners;
    ListenerList<Listener > lastListeners;
    
};




#endif  // QUEUEDNOTIFIER_H_INCLUDED
