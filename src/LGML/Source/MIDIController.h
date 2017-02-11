/*
  ==============================================================================

    MIDIController.h
    Created: 2 Mar 2016 8:51:20pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef MIDICONTROLLER_H_INCLUDED
#define MIDICONTROLLER_H_INCLUDED

#include "Controller.h"
#include "MIDIListener.h"
#include "JsEnvironment.h"
class JsMIDIMessageListener;


class MIDIController : public Controller,public MIDIListener, public MIDIListener::Listener,public JsEnvironment
{
public :
    MIDIController();
	virtual ~MIDIController();

  ControllerEditor *  getEditor()override;

    // should be implemented to build localenv
    void buildLocalEnv() override;
	void handleIncomingMidiMessage(MidiInput* source,
		const MidiMessage& message) override;

    StringParameter * deviceInName;

    
    BoolParameter * logIncoming;
	IntParameter * channelFilter;


	MidiMessageCollector midiCollector;

	void currentDeviceChanged(MIDIListener * m) override;

	
    void    onContainerParameterChanged(Parameter * )override;

    static var sendCCFromJS(const var::NativeFunctionArgs & v);
    static var sendNoteOnFromJS(const var::NativeFunctionArgs & v);
	static var sendNoteOffFromJS(const var::NativeFunctionArgs & v);
	static var sendSysExFromJS(const var::NativeFunctionArgs &v);

    void callJs(const MidiMessage& message);
    void newJsFileLoaded()override;

	class MIDIControllerListener
	{
	public:
		virtual ~MIDIControllerListener() {}
		virtual void midiMessageReceived(const MidiMessage&) {}
	};

	ListenerList<MIDIControllerListener> midiControllerListeners;
	void addMIDIControllerListener(MIDIControllerListener* newListener) { midiControllerListeners.add(newListener); }
	void removeMIDIControllerListener(MIDIControllerListener* listener) { midiControllerListeners.remove(listener); }


  static var createJsNoteListener(const var::NativeFunctionArgs &);
  static var createJsCCListener(const var::NativeFunctionArgs &);

  OwnedArray<JsMIDIMessageListener,CriticalSection> jsNoteListeners;
  OwnedArray<JsMIDIMessageListener,CriticalSection> jsCCListeners;


  // from jsenvironment
  void clearNamespace()override;

private:
  

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIController)
};

class JsMIDIMessageListener{
  public :
  JsMIDIMessageListener(JsEnvironment * js,int _channel ,int _numberToListen,bool _isNoteListener):jsEnv(js),
  numberToListen(_numberToListen),
  channel(_channel),
  isNoteListener(_isNoteListener){
    buildVarObject();

  }
  static Identifier midiReceivedId;
static Identifier midiValueId;

  virtual ~JsMIDIMessageListener(){};
  void buildVarObject(){
      object= new DynamicObject();
      DynamicObject * dob = object.getDynamicObject();
      dob->setProperty(midiValueId,0);
      dob->setMethod(midiReceivedId,&JsMIDIMessageListener::dummyCallback);

  }

  // overriden in Js
#pragma warning(push)
#pragma warning(disable:4305 4800)
  static var dummyCallback(const var::NativeFunctionArgs & /*a*/)
  {
	  return var::undefined;
  };
#pragma warning(pop)

  void processMessage(const MidiMessage & m)
  {
    if(channel == 0 || channel == m.getChannel()){
      if((isNoteListener && m.isNoteOnOrOff()) || (m.isController())){
      int numToTest = isNoteListener? m.getNoteNumber():m.getControllerNumber();
        if(numToTest == numberToListen){
          var value = isNoteListener?m.getVelocity():m.getControllerNumber();
          object.getDynamicObject()->setProperty(midiValueId,0);
          jsEnv->callFunctionFromIdentifier(midiReceivedId, var::NativeFunctionArgs(object,&value,1), true);
        }

      }
    }
  };
  JsEnvironment* jsEnv;
  int channel;
  // can be CC or note number
  int numberToListen;

  bool isNoteListener;
  var object;
};


#endif  // MIDICONTROLLER_H_INCLUDED
