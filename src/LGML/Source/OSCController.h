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




    StringParameter * localPortParam;
    StringParameter * remoteHostParam;
    StringParameter * remotePortParam;
    BoolParameter * logIncomingOSC;
    BoolParameter * logOutGoingOSC;
    
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

#if JUCE_COMPILER_SUPPORTS_VARIADIC_TEMPLATES && JUCE_COMPILER_SUPPORTS_MOVE_SEMANTICS
    template <typename... Args>
    bool sendOSC (const OSCAddressPattern& address, Args&&... args)
    {
        OSCMessage m = OSCMessage (address, std::forward<Args> (args)...);
        if(logOutGoingOSC->boolValue()){logMessage(m);}
        return sender.send (m);
    }
#endif
    
    bool sendOSC (OSCMessage & m)
    {
        if(logOutGoingOSC->boolValue()){logMessage(m);}
        return sender.send (m);
    }

    void logMessage(const OSCMessage & m,const String & prefix = "");

private:
    // should use sendOSC for centralizing every call
    OSCReceiver receiver;
    OSCSender sender;
};

#endif  // OSCCONTROLLER_H_INCLUDED
