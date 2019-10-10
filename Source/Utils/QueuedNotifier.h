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

#ifndef QUEUEDNOTIFIER_H_INCLUDED
#define QUEUEDNOTIFIER_H_INCLUDED
#pragma once

#include "../JuceHeaderCore.h"//keep


template<typename MessageClass, class CriticalSectionToUse = CriticalSection>
class QueuedNotifier: public  AsyncUpdater
{
public:


    QueuedNotifier (int _maxSize, bool _canDropMessage = true): fifo (_maxSize), canDropMessage (_canDropMessage)
    {
        maxSize = _maxSize;
        messageQueue.ensureStorageAllocated (maxSize);
        notifierQueue.ensureStorageAllocated(maxSize);

    }

    virtual ~QueuedNotifier() {cancelPendingUpdate();masterReference.clear();}

    using thisQueueType = QueuedNotifier<MessageClass,CriticalSectionToUse> ;
    typename WeakReference<thisQueueType>::Master  masterReference;

    class Listener
    {
    public:
        virtual ~Listener() {
            while(linkedQ.size()){
                if(auto l = linkedQ.getLast().get()){
                    l->removeListener(this);
                }
                else{
                linkedQ.removeLast();
                }

            }
        }
        virtual void newMessage (const MessageClass&) = 0;
        Array<WeakReference<thisQueueType>> linkedQ;
    };



    void addMessage (MessageClass* msg, bool forceSendNow = false,Listener * notifier=nullptr)
    {
        if (listeners.size() == 0 && lastListeners.size() == 0)
        {
            delete msg;
            return;
        }

        forceSendNow |= MessageManager::getInstance()->isThisTheMessageThread();

//        if (forceSendNow)
//        {
//            listeners.callExcluding(notifier,&Listener::newMessage, *msg);
//            lastListeners.callExcluding (notifier,&Listener::newMessage, *msg);
//            delete msg;
//            return;
//        }
//        else
//        {


            int start1, size1, start2, size2;
            fifo.prepareToWrite (1, start1, size1, start2, size2);

            // fifo is full : we can drop message or wait
            while (size1 == 0)
            {
                if (canDropMessage)
                {

                    fifo.finishedRead (1);
                    fifo.prepareToWrite (1, start1, size1, start2, size2);

                }
                else
                {
                    fifo.prepareToWrite (1, start1, size1, start2, size2);
                    Thread::sleep (10);
                }
            }




            jassert (size1 == 1 && size2 == 0);

            if (messageQueue.size() < maxSize) {
                messageQueue.add (msg);
                notifierQueue.add(notifier);
            }
            else {
                messageQueue.set (start1, msg);
                notifierQueue.set(start1,notifier);
            }

            fifo.finishedWrite (size1 );
        if(forceSendNow){
            handleAsyncUpdate();
        }
        else{
            triggerAsyncUpdate();
        }
//        }

    }





    // allow to stack all values or get only last updated value
    void addListener (Listener* newListener) { listeners.add (newListener);newListener->linkedQ.add(this); }
    void addAsyncCoalescedListener (Listener* newListener) { lastListeners.add (newListener);newListener->linkedQ.add(this); }
    void removeListener (Listener* listener) { listeners.remove (listener); lastListeners.remove (listener);listener->linkedQ.removeAllInstancesOf(this); }
private:

    void handleAsyncUpdate() override
    {



        int start1, size1, start2, size2;
        fifo.prepareToRead (fifo.getNumReady(), start1, size1, start2, size2);

        for (int i = start1 ; i < start1 + size1 ; i++)
        {
            listeners.callExcluding (notifierQueue.getUnchecked(i),&Listener::newMessage, *messageQueue.getUnchecked (i));

        }

        for (int i = start2 ; i < start2 + size2 ; i++)
        {
            listeners.callExcluding (notifierQueue.getUnchecked(i),&Listener::newMessage, *messageQueue.getUnchecked (i));
        }


        if (size2 > 0)
        {
            int i =start2 + size2 - 1;
            lastListeners.callExcluding (notifierQueue.getUnchecked(i),&Listener::newMessage, *messageQueue.getUnchecked (i));
        }
        else if (size1 > 0)
        {
            int i = start1 + size1 - 1;
            lastListeners.callExcluding (notifierQueue.getUnchecked(i),&Listener::newMessage, *messageQueue.getUnchecked (i));
        }

        fifo.finishedRead (size1 + size2);




    }



    AbstractFifo fifo;
    int maxSize;
    OwnedArray<MessageClass, CriticalSectionToUse> messageQueue;
    Array<Listener*,CriticalSectionToUse> notifierQueue;
    bool canDropMessage;
    ListenerList<Listener > listeners;
    ListenerList<Listener > lastListeners;


};




#endif  // QUEUEDNOTIFIER_H_INCLUDED
