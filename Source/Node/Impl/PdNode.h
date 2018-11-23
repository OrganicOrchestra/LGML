/*
  ==============================================================================

    PdNode.h
    Created: 20 Nov 2018 2:41:30am
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

#include "../NodeBase.h"
#include "../../../third_party/libpd/libpd_wrapper/z_libpd.h"

#include "../../MIDI/MIDIListener.h"
#include "../../MIDI/MIDIHelpers.h"

class PdNode:public NodeBase,public MIDIListener{
public:
    DECLARE_OBJ_TYPE (PdNode,"load Pd Patch");

    ~PdNode();

    StringParameter* pdPath;
    BoolParameter *isLoadedParam;
    Trigger *reloadPatch;
    Array<ParameterBase*> pdParameters;

    MIDIHelpers::MIDIIOChooser midiChooser;

    void numChannelsChanged (bool isInput) override;


    void processBlockInternal (AudioBuffer<float>& buffer, MidiBuffer&)override;
    void onContainerParameterChanged ( ParameterBase* p) override;
    
    void prepareToPlay(double , int) override;
    bool isLoaded();

    Trigger* midiActivityTrigger;

    void onContainerTriggerTriggered(Trigger *t)override;


private:


    void parseParameters();

    void unloadFile();
    HeapBlock<float> tempInBuf,tempOutBuf;

    void loadValidPdFile();
    t_pdinstance * pdinstance;
    void * patchHandle;
    int numTicks;
    int dollarZero;


    void handleIncomingMidiMessage (MidiInput* source,
                                    const MidiMessage& message) override;

    MidiMessageCollector messageCollector;
    MidiBuffer incomingMidi;

};
