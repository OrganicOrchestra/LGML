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


#ifndef QUEUEDNOTIFIER_H_INCLUDED
#define QUEUEDNOTIFIER_H_INCLUDED
#pragma once

#include "../JuceHeaderCore.h"//keep


template<typename MessageClass,class CriticalSectionToUse = CriticalSection>
class QueuedNotifier:public  AsyncUpdater{
public:

  QueuedNotifier(int _maxSize,bool _canDropMessage=true):fifo(_maxSize),canDropMessage(_canDropMessage){
    maxSize = _maxSize;
    messageQueue.ensureStorageAllocated(maxSize);

  }

  virtual ~QueuedNotifier() {cancelPendingUpdate();}



  class Listener{
  public:
    virtual ~Listener(){}
    virtual void newMessage(const MessageClass&) = 0;
  };



  void addMessage(MessageClass * msg,bool forceSendNow = false){
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

      // add if we are in a decent array size

      int start1,size1,start2,size2;
      fifo.prepareToWrite(1, start1, size1, start2, size2);

      // fifo is full : we can drop message
      while(size1==0){
//        jassert(isUpdatePending());
        if(canDropMessage){
          fifo.finishedRead(1);
          fifo.prepareToWrite(1, start1, size1, start2, size2);

        }
        else{
            fifo.prepareToWrite(1, start1, size1, start2, size2);
            Thread::sleep(10);
          }
        }
      



          jassert(size1==1 && size2==0);
        if(messageQueue.size()<maxSize){messageQueue.add(msg);}
        else{messageQueue.set(start1,msg);}

      fifo.finishedWrite (size1 );
      triggerAsyncUpdate();
    }

  }





  // allow to stack all values or get only last updated value
  void addListener(Listener* newListener) { listeners.add(newListener); }
  void addAsyncCoalescedListener(Listener* newListener) { lastListeners.add(newListener); }
  void removeListener(Listener* listener) { listeners.remove(listener);lastListeners.remove(listener); }
private:

  void handleAsyncUpdate() override
  {



    int start1,size1,start2,size2;
    fifo.prepareToRead(fifo.getNumReady(), start1, size1, start2, size2);

    for(int i = start1 ;i <start1+ size1 ; i++){
      listeners.call(&Listener::newMessage,*messageQueue.getUnchecked(i));

    }

    for(int i = start2 ;i <start2+ size2 ; i++){
      listeners.call(&Listener::newMessage,*messageQueue.getUnchecked(i));
    }


    if(size2>0){
      lastListeners.call(&Listener::newMessage,*messageQueue.getUnchecked(start2+size2-1));}
    else if(size1>0){
      lastListeners.call(&Listener::newMessage,*messageQueue.getUnchecked(start1+size1-1));}

    fifo.finishedRead(size1 + size2);




  }



  AbstractFifo fifo;
  int maxSize;
  OwnedArray<MessageClass,CriticalSectionToUse> messageQueue;
  bool canDropMessage;
  ListenerList<Listener > listeners;
  ListenerList<Listener > lastListeners;
  
};




#endif  // QUEUEDNOTIFIER_H_INCLUDED
