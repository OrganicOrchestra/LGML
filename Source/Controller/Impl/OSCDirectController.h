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


#ifndef OSCDIRECTCONTROLLER_H_INCLUDED
#define OSCDIRECTCONTROLLER_H_INCLUDED

#include "OSCController.h"

class OSCDirectController : public OSCController
{
public:
    OSCDirectController();
	OSCDirectController(const String &name);
	virtual ~OSCDirectController();

  BoolParameter * sendTimeInfo;


    virtual Result processMessageInternal(const OSCMessage &msg) override;

  


    // Inherited via Listener
    virtual void controllableAdded(ControllableContainer *,Controllable * c) override;
    virtual void controllableRemoved(ControllableContainer *,Controllable * c) override;
    virtual void controllableContainerAdded(ControllableContainer *,ControllableContainer * cc) override;
    virtual void controllableContainerRemoved(ControllableContainer *,ControllableContainer * cc) override;

    virtual void controllableFeedbackUpdate(ControllableContainer *originContainer,Controllable * c) override;

    virtual void onContainerParameterChanged(Parameter * p) override;




        //Listener
        class  OSCDirectListener
        {
        public:
            /** Destructor. */
            virtual ~OSCDirectListener() {}
            virtual void messageProcessed(const OSCMessage & msg, Result success) = 0;
        };

        ListenerList<OSCDirectListener> oscDirectlisteners;
        void addOSCDirectParameterListener(OSCDirectListener* newListener) { oscDirectlisteners.add(newListener); }
        void removeOSCDirectParameterListener(OSCDirectListener* listener) { oscDirectlisteners.remove(listener); }



private:
    void sendOSCForAddress(Controllable*,const String & address);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCDirectController)


        // Inherited via Listener


};


#endif  // OSCDIRECTCONTROLLER_H_INCLUDED
