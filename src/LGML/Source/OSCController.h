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
class StringParameter;

#define NUM_OSC_MSG_IN_A_ROW 10
#define OSC_QUEUE_LENGTH 5000


class OSCController : public Controller, public OSCReceiver::Listener<OSCReceiver::RealtimeCallback>
{
public:
  OSCController(const String &name);
  virtual ~OSCController();

  StringParameter * localPortParam;
  StringParameter * remoteHostParam;
  StringParameter * remotePortParam;
  FloatParameter *speedLimit;
  BoolParameter * logIncomingOSC;
  BoolParameter * logOutGoingOSC;
  BoolParameter * blockFeedback;// if a parameter is updated from processOSC , stops any osc out with same address
  Trigger * sendAllParameters;
  
  

  float lastOSCMessageSentTime;
  int numSentInARow;

  void setupReceiver();
  void setupSender();

  void processMessage(const OSCMessage & msg);
  virtual Result processMessageInternal(const OSCMessage &msg);



  virtual void onContainerParameterChanged(Parameter * p) override;
  virtual void onContainerTriggerTriggered(Trigger *t) override;

  virtual void oscMessageReceived(const OSCMessage & message) override;
  virtual void oscBundleReceived(const OSCBundle& bundle) override;
  void sendAllControllableStates(ControllableContainer *c,int & sentControllable );

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
    OSCMessage m =OSCMessage (address, std::forward<Args> (args)...);
    return sendOSC(m);
  }
#endif

  class OSCMessageQueue:public Timer{
  public:
    OSCMessageQueue(OSCController* o);
    void add(OSCMessage * m);
    void timerCallback() override;
    Array<OSCMessage*> messages;
    AbstractFifo aFifo;
    OSCController* owner;
    int interval;

  };

  OSCMessageQueue oscMessageQueue;
  bool sendOSC (OSCMessage & m);

  void logMessage(const OSCMessage & m,const String & prefix = "");

private:
  bool sendOSCInternal(OSCMessage & m);
  friend class OSCMessageQueue;
  // should use sendOSC for centralizing every call
  OSCReceiver receiver;
  OSCSender sender;
//  String lastAddressReceived;
  OSCMessage lastMessageReceived;
  bool isProcessingOSC;
};




#endif  // OSCCONTROLLER_H_INCLUDED
