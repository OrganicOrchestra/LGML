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

class MIDIController : public Controller,public MIDIListener,public JsEnvironment
{
public :
    MIDIController();
	virtual ~MIDIController();

    // should be implemented to build localenv
    void buildLocalEnv() override;
	void handleIncomingMidiMessage(MidiInput* source,
		const MidiMessage& message) override;
    StringParameter * deviceInName;
    StringParameter * scriptPath;
    BoolParameter * logIncoming;


  MidiMessageCollector midiCollector;





	ControllerUI * createUI() override;
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

private:
  

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIController)
};


#endif  // MIDICONTROLLER_H_INCLUDED
