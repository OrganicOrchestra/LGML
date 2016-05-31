/*
  ==============================================================================

    OSCDirectController.h
    Created: 8 Mar 2016 10:27:37pm
    Author:  bkupe

  ==============================================================================
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


    virtual Result processMessageInternal(const OSCMessage &msg) override;


	
    // Inherited via Listener
    virtual void controllableAdded(Controllable * c) override;
    virtual void controllableRemoved(Controllable * c) override;
    virtual void controllableContainerAdded(ControllableContainer * cc) override;
    virtual void controllableContainerRemoved(ControllableContainer * cc) override;

    virtual void controllableFeedbackUpdate(Controllable * c) override;



    public:
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




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCDirectController)


        // Inherited via Listener


};


#endif  // OSCDIRECTCONTROLLER_H_INCLUDED
