/*
 ==============================================================================

 LooperNode.h
 Created: 3 Mar 2016 10:32:16pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef LOOPERNODE_H_INCLUDED
#define LOOPERNODE_H_INCLUDED


#include "NodeBase.h"
#include "AudioHelpers.h"
#include "LooperTrack.h"

#define MAX_NUM_TRACKS 32

class LooperNode : public NodeBase
{

public:
    LooperNode(uint32 nodeId);
	virtual ~LooperNode();

    NodeBaseUI * createUI() override;

    class TrackGroup : public ControllableContainer{
    public:
        TrackGroup(LooperNode* l):ControllableContainer("tracks"),owner(l){};

        	void setNumTracks(int numTracks);
        void addTrack();
        void removeTrack(int i);


	OwnedArray<LooperTrack> tracks;
        LooperNode * owner;

    };

    TrackGroup trackGroup;
	LooperTrack * selectedTrack;

	LooperTrack * lastMasterTempoTrack;

	//Parameters
	Trigger * recPlaySelectedTrig;
	Trigger * playSelectedTrig;
	Trigger * clearSelectedTrig;
	Trigger * stopSelectedTrig;

	Trigger * selectAllTrig;
	Trigger * clearAllTrig;
	Trigger * stopAllTrig;

	FloatParameter * volumeSelected;
	BoolParameter * isMonitoring;
	IntParameter * numberOfTracks;
    IntParameter * selectTrack;


	AudioBuffer<float> bufferIn;
	AudioBuffer<float>bufferOut;




	void selectMe(LooperTrack * t);


	bool askForBeingMasterTrack(LooperTrack * t);
	bool askForBeingAbleToPlayNow(LooperTrack *_t);
	bool areAllTrackClearedButThis(LooperTrack * _t);


	void onContainerTriggerTriggered(Trigger * t) override;
	void onContainerParameterChanged(Parameter * p) override;
	// internal
	void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)override;
	void checkIfNeedGlobalLooperStateUpdate();

	bool wasMonitoring;

	//Listener
	class  LooperListener
	{
	public:

		/** Destructor. */
		virtual ~LooperListener() {}
		virtual void trackNumChanged(int num) = 0;
	};

	ListenerList<LooperListener> looperListeners;
	void addLooperListener(LooperListener* newListener) { looperListeners.add(newListener); }
	void removeLooperListener(LooperListener* listener) { looperListeners.remove(listener); }


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LooperNode)
};

#endif  // LOOPERNODE_H_INCLUDED
