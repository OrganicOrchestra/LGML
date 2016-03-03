/*
  ==============================================================================

    NodeBase.h
    Created: 2 Mar 2016 8:36:17pm
    Author:  bkupe

  ==============================================================================
*/


#ifndef NODEBASE_H_INCLUDED
#define NODEBASE_H_INCLUDED

#include <JuceHeader.h>
#include "DataProcessor.h"

class NodeBase : public ReferenceCountedObject
{

public:
	class NodeAudioProcessor : public juce::AudioProcessor
	{
	public:
		NodeAudioProcessor() :AudioProcessor() {};

		
		virtual const String getName() const { return "NodeBase"; };
		virtual void prepareToPlay(double sampleRate,int estimatedSamplesPerBlock) {jassert(false);};
		virtual void releaseResources() {jassert(false);};



		bool silenceInProducesSilenceOut() const { return false; }



		virtual AudioProcessorEditor* createEditor() { return nullptr; }
		virtual bool hasEditor() const { return false; }



		// dumb overrides from JUCE AudioProcessor :  MIDI
		int getNumPrograms() { return 0; }
		int getCurrentProgram() { return 0; }
		void setCurrentProgram(int index) {}
		const String getProgramName(int index) { return "NoProgram"; }
		void changeProgramName(int index, const String& newName) {};
		double getTailLengthSeconds() const { return 0; }
		bool acceptsMidi() const { return false; }
		bool producesMidi() const { return false; }


		// save procedures from host
		virtual void getStateInformation(juce::MemoryBlock& destData) {};
		virtual void setStateInformation(const void* data, int sizeInBytes){};



		virtual void processBlock(AudioBuffer<float>& buffer,
			MidiBuffer& midiMessages) = 0;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeAudioProcessor)
	};


	class NodeDataProcessor : public DataProcessor
	{
	public:
		NodeDataProcessor() {};

		virtual void processData(Data * incomingData, String targetInputDataName, String targetDataComponentName) = 0;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeDataProcessor)
	};


	

	NodeBase(uint32 nodeId, NodeBase::NodeAudioProcessor * audioProcessor = nullptr, NodeBase::NodeDataProcessor * dataProcessor = nullptr);

	uint32 nodeId;

	bool hasAudioInputs;
	bool hasAudioOutputs;
	bool hasDataInputs;
	bool hasDataOutputs;

	ScopedPointer<NodeAudioProcessor> audioProcessor;
	ScopedPointer<NodeDataProcessor> dataProcessor;


	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeBase)
};

#endif  // NODEBASE_H_INCLUDED
