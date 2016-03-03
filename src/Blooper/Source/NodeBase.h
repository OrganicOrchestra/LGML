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

class NodeBase
{

public:
	class NodeAudioProcessor : public AudioProcessor
	{
	public:
		NodeAudioProcessor() :AudioProcessor() {};


		virtual const String getName() const { return "NodeBase"; };

		virtual void prepareToPlay(double sampleRate,
			int estimatedSamplesPerBlock) {
			jassert(false);
		};

		virtual void releaseResources() {
			jassert(false);
		};



		/** Returns true if a silent input always produces a silent output. */
		virtual bool silenceInProducesSilenceOut() const { return false; }

		/** Returns the length of the filter's tail, in seconds. */
		virtual double getTailLengthSeconds() const { return 0; }

		/** Returns true if the processor wants midi messages. */
		virtual bool acceptsMidi() const { return false; }

		/** Returns true if the processor produces midi messages. */
		virtual bool producesMidi() const { return false; }


		virtual AudioProcessorEditor* createEditor() { return nullptr; }

		/** Your filter must override this and return true if it can create an editor component.
		@see createEditor
		*/
		virtual bool hasEditor() const { return false; }



		virtual int getNumPrograms() { return 0; }
		/** Returns the number of the currently active program. */
		virtual int getCurrentProgram() { return 0; }

		/** Called by the host to change the current program. */
		virtual void setCurrentProgram(int index) {}

		/** Must return the name of a given program. */
		virtual const String getProgramName(int index) { return "NoProgram"; }

		/** Called by the host to rename a program. */
		virtual void changeProgramName(int index, const String& newName) {};


		virtual void getStateInformation(juce::MemoryBlock& destData) {};

		virtual void setStateInformation(const void* data, int sizeInBytes){};

		virtual void processBlock(AudioBuffer<float>& buffer,
			MidiBuffer& midiMessages) = 0;

	};


	class NodeDataProcessor // : DataProcessor
	{
	public:
		NodeDataProcessor() {};



		virtual int getTotalNumInputChannels() = 0;
		virtual int getTotalNumOutputChannels() = 0;

	};

	NodeBase(NodeBase::NodeAudioProcessor * audioProcessor = nullptr, NodeBase::NodeDataProcessor * dataProcessor = nullptr);

	bool hasAudioInputs;
	bool hasAudioOutputs;
	bool hasDataInputs;
	bool hasDataOutputs;

	ScopedPointer<NodeAudioProcessor> audioProcessor;
	ScopedPointer<NodeDataProcessor> dataProcessor;


	

};

#endif  // NODEBASE_H_INCLUDED
