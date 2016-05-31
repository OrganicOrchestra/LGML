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

    QueuedNotifier(int _maxSize){
        maxSize = _maxSize;
        writeIdx = 0;
    }

    virtual ~QueuedNotifier() {cancelPendingUpdate();}



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
            return;
        }
        else{
            if(listeners.size()==0){
                const ScopedLock lk(mutex);
                writeIdx = 0;
            }
            // add if we are in a decent array size
            if(messageQueue.size()<maxSize && writeIdx>messageQueue.size()){messageQueue.add(msg);}
            else{messageQueue.set(writeIdx%messageQueue.size(),msg);}

            {
                    const ScopedLock lk(mutex);
                    writeIdx++;
            }


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
        const ScopedLock lk(mutex);
        int start = 0;
        int end =writeIdx;
        if(writeIdx>messageQueue.size()){
            start =  writeIdx-messageQueue.size();
            end = writeIdx-1;
        }

        {
            const typename CriticalSectionToUse::ScopedLockType _lk(messageQueue.getLock());
            for(int i = start ; i < end ; i++){
                listeners.call(&Listener::newMessage,*messageQueue.getUnchecked(i));
            }
            if(writeIdx>0)lastListeners.call(&Listener::newMessage,*messageQueue.getUnchecked(end));
        }
        writeIdx = 0;


    }


    int writeIdx,maxSize;
    CriticalSection mutex;
    OwnedArray<MessageClass,CriticalSectionToUse> messageQueue;

    ListenerList<Listener > listeners;
    ListenerList<Listener > lastListeners;

};




#endif  // QUEUEDNOTIFIER_H_INCLUDED
