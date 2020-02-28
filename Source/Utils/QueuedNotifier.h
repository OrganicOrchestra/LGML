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


#pragma once

#include "../JuceHeaderCore.h"//keep

//template<typename MessageClass, class CriticalSectionToUse>
//class QueuedNotifier;
//
//template<typename MessageClass>
//class QueuedNotifierListener;

struct QNPrivateData;


template<typename MessageClass,class CriticalSectionToUse = DummyCriticalSection>
class QueuedNotifier: public  AsyncUpdater
{
public:

    bool debugFlag = false;
    QueuedNotifier (int _maxSize, bool _canDropMessage = true,String _name="");

    virtual ~QueuedNotifier() ;

    using thisQueueType = QueuedNotifier<MessageClass,CriticalSectionToUse> ;
    typename WeakReference< thisQueueType >::Master  masterReference;

    bool isNotifying() const ;

    class Listener
    {
    public:
        virtual ~Listener() ;
        virtual void newMessage (const MessageClass&) = 0;
        Array< WeakReference< thisQueueType > > linkedQ;



    };



    
    void addMessage (MessageClass* msg, bool forceSendNow = false,Listener * notifier=nullptr);

    String name;



    // allow to stack all values or get only last updated value
    void addListener (Listener* newListener) ;
    void addAsyncCoalescedListener (Listener* newListener) ;
    void removeListener (Listener* listener) ;
private:
    
    void handleAsyncUpdate() override;

    std::unique_ptr< QNPrivateData > _data;


    AbstractFifo fifo;
    int maxSize;
    OwnedArray<MessageClass, CriticalSectionToUse> messageQueue;
    Array<Listener*,CriticalSectionToUse> notifierQueue;
    bool canDropMessage;
    ListenerList<Listener > listeners;
    ListenerList<Listener > lastListeners;


};


