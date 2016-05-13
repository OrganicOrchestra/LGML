/*
  ==============================================================================

    OSCController.h
    Created: 2 Mar 2016 8:50:08pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef OSCCONTROLLER_H_INCLUDED
#define OSCCONTROLLER_H_INCLUDED

#include "Controller.h"
#include "StringParameter.h"


class OSCController : public Controller, public OSCReceiver::Listener<OSCReceiver::MessageLoopCallback>
{
public:
    OSCController(const String &name);
    virtual ~OSCController();


    OSCReceiver receiver;
    OSCSender sender;

    StringParameter * localPortParam;
    StringParameter * remoteHostParam;
    StringParameter * remotePortParam;

    void setupReceiver();
    void setupSender();

    void processMessage(const OSCMessage & msg);
	virtual Result processMessageInternal(const OSCMessage &msg);



    virtual void onContainerParameterChanged(Parameter * p) override;

	virtual void oscMessageReceived(const OSCMessage & message) override;
	virtual void oscBundleReceived(const OSCBundle& bundle) override;


	virtual ControllerUI * createUI() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCController)

		//Listener
	class  OSCControllerListener
	{
	public:
		/** Destructor. */
		virtual ~OSCControllerListener() {}
		virtual void messageProcessed(const OSCMessage & msg, bool success) = 0;
	};

	ListenerList<OSCControllerListener> oscListeners;
	void addOSCControllerListener(OSCControllerListener* newListener) { oscListeners.add(newListener); }
	void removeOSCControllerListener(OSCControllerListener* listener) { oscListeners.remove(listener); }
};

#endif  // OSCCONTROLLER_H_INCLUDED
