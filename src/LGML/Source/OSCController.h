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

    virtual void processMessage(const OSCMessage & msg);

    virtual void parameterValueChanged(Parameter * p) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCController)

    // Inherited via Listener
private:
    virtual void oscMessageReceived(const OSCMessage & message) override;
    virtual void oscBundleReceived(const OSCBundle& bundle) override {
		
        for (auto &m : bundle)
        {
            processMessage(m.getMessage());
        }
    }
};



#endif  // OSCCONTROLLER_H_INCLUDED
