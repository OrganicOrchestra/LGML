/*
 ==============================================================================

 QueuedNotifier.cpp
 Created: 3 Feb 2020 2:40:21pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "QueuedNotifier.h"


struct QNPrivateData{
    void* tid=nullptr;
    bool isNotifying = false;
    Atomic<unsigned long> numSent=0;
};

#define DBGQUEUED 0



template<typename MessageClass, class CriticalSectionToUse >
QueuedNotifier<MessageClass,CriticalSectionToUse>::QueuedNotifier (int _maxSize, bool _canDropMessage ,String _name): fifo (_maxSize), canDropMessage (_canDropMessage),name(_name),_data(std::make_unique<QNPrivateData>())
{
    maxSize = _maxSize;
    messageQueue.ensureStorageAllocated (maxSize);
    notifierQueue.ensureStorageAllocated(maxSize);
    for(int i = 0; i < maxSize; i++){
        messageQueue.add (nullptr);
        notifierQueue.add(nullptr);
    }

}

template<typename MessageClass, class CriticalSectionToUse >
QueuedNotifier<MessageClass,CriticalSectionToUse>::~QueuedNotifier() {
//    if(DBGQUEUED && debugFlag){
//        NLOG(name,"ooooo deleting Me"+String(fifo.getNumReady()));
//    }
    cancelPendingUpdate();
    QueuedNotifier<MessageClass,CriticalSectionToUse>::masterReference.clear();

}







template<typename MessageClass, class CriticalSectionToUse >
void QueuedNotifier<MessageClass,CriticalSectionToUse>::addMessage (MessageClass* msg, bool forceSendNow ,Listener * notifier)
{

    if(DBGQUEUED && debugFlag){
        if(_data->tid!=Thread::getCurrentThreadId()){
            auto jt = Thread::getCurrentThread();
            auto tname = jt?jt->getThreadName():"unknown";
            NLOG(name,"ooooo calling from various thread : "+tname+" : "+(isNotifying()?"notifying":""));
//            if(_data->tid){
//                int dbg;
//                dbg++;
//            }
//            if(isNotifying()){
//                int dbg;
//                dbg++;
//            }
        }
        _data->tid =Thread::getCurrentThreadId();
    }

    if (listeners.size() == 0 && lastListeners.size() == 0)
    {
//        if(DBGQUEUED && debugFlag){
//            NLOG(name,"ooooo deleting Msg"+String(fifo.getNumReady())+","+String(_data->numSent.get()));
//        }
        delete msg;
        return;
    }

    forceSendNow |= (!isNotifying() && MessageManager::getInstance()->isThisTheMessageThread());



    // fifo is full : we can drop message or wait
    while (fifo.getFreeSpace() == 0)
    {
        if(DBGQUEUED && debugFlag){
            NLOG(name,"ooooo fifo full"+String(fifo.getNumReady())+","+String(_data->numSent.get()));
        }
        if (canDropMessage)
        {
            auto rHdl = fifo.read(1);
            if(DBGQUEUED && debugFlag){
                NLOG(name,"ooooo fifo drop"+String(fifo.getNumReady())+","+String(_data->numSent.get()));
            }

        }
        else
        {
            Thread::sleep (10);
        }
    }
    int start1, size1, start2, size2;
    fifo.prepareToWrite (1, start1, size1, start2, size2);

    jassert (size1 == 1 && size2 == 0);

    if (messageQueue.size() < maxSize) {
        jassert(start1==messageQueue.size());
//        if(DBGQUEUED && debugFlag){
//            NLOG(name,"ooooo adding Msg"+String(fifo.getNumReady())+","+String(_data->numSent.get()));
//        }
        messageQueue.add (msg);
        notifierQueue.add(notifier);
    }
    else {
//        if(DBGQUEUED && debugFlag){
//            NLOG(name,"ooooo setting Msg"+String(fifo.getNumReady())+","+String(_data->numSent.get()));
//        }
        messageQueue.set (start1, msg);
        notifierQueue.set(start1,notifier);
    }

    fifo.finishedWrite (size1 );
//#if DBGQUEUED
    ++_data->numSent;
//#endif
    if(forceSendNow){
        handleAsyncUpdate();
    }
    else{
        triggerAsyncUpdate();
    }


}

template<typename MessageClass, class CriticalSectionToUse >
QueuedNotifier<MessageClass,CriticalSectionToUse>::Listener::~Listener(){
    while(linkedQ.size()){
        if(auto l = linkedQ.getLast().get()){
            l->removeListener(this);
        }
        else{
            linkedQ.removeLast();
        }

    }
}


template<typename MessageClass, class CriticalSectionToUse >
void QueuedNotifier<MessageClass,CriticalSectionToUse>::addListener (Listener* newListener) {
    listeners.add (newListener);
    newListener->linkedQ.add(this);

}

template<typename MessageClass, class CriticalSectionToUse >
void QueuedNotifier<MessageClass,CriticalSectionToUse>::addAsyncCoalescedListener (Listener* newListener) {
    lastListeners.add (newListener);
    newListener->linkedQ.add(this);


}
template<typename MessageClass, class CriticalSectionToUse >
void QueuedNotifier<MessageClass,CriticalSectionToUse>::removeListener (Listener* listener) {
    listeners.remove (listener); lastListeners.remove (listener);
    listener->linkedQ.removeAllInstancesOf(this);

}

template<typename MessageClass, class CriticalSectionToUse >
void QueuedNotifier<MessageClass,CriticalSectionToUse>::handleAsyncUpdate()
{
//    if(DBGQUEUED && debugFlag){
//        NLOG(name,"ooooo update"+String(fifo.getNumReady())+","+String(_data->numSent.get()));
//    }



    int start1, size1, start2, size2;
    fifo.prepareToRead (fifo.getNumReady(), start1, size1, start2, size2);
    if(size1+size2>_data->numSent.get()){
        fifo.finishedRead(size1+size2);
        triggerAsyncUpdate();
        NLOGE(name,"oooo QueuedNotifier corrupted , "+String(_data->numSent.get()));
        jassertfalse;
        return;

    }
    _data->isNotifying = true;
    for (int i = start1 ; i < start1 + size1 ; i++)
    {
        if(messageQueue.getUnchecked (i)!=nullptr){
            listeners.callExcluding (notifierQueue.getUnchecked(i),&Listener::newMessage, *messageQueue.getUnchecked (i));
        }
        else{jassertfalse;break;}
    }

    for (int i = start2 ; i < start2 + size2 ; i++)
    {
        if(messageQueue.getUnchecked (i)!=nullptr){
            listeners.callExcluding (notifierQueue.getUnchecked(i),&Listener::newMessage, *messageQueue.getUnchecked (i));
        }
        else{jassertfalse;break;}
    }


    if (size2 > 0)
    {
        int i =start2 + size2 - 1;
        if(messageQueue.getUnchecked (i)!=nullptr){
            lastListeners.callExcluding (notifierQueue.getUnchecked(i),&Listener::newMessage, *messageQueue.getUnchecked (i));
        }
        else{jassertfalse;}
    }
    else if (size1 > 0)
    {
        int i = start1 + size1 - 1;
        if(messageQueue.getUnchecked (i)!=nullptr){
            lastListeners.callExcluding (notifierQueue.getUnchecked(i),&Listener::newMessage, *messageQueue.getUnchecked (i));
        }
        else{jassertfalse;}
    }
    fifo.finishedRead (size1 + size2);
    _data->isNotifying = false;



}

template<typename MessageClass, class CriticalSectionToUse >
bool QueuedNotifier<MessageClass,CriticalSectionToUse>::isNotifying() const {
    return _data->isNotifying;
}





