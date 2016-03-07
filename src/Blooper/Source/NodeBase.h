/*
  ==============================================================================

    NodeBase.h
    Created: 2 Mar 2016 8:36:17pm
    Author:  bkupe

  ==============================================================================
*/

/*
NodeBase is the base class for all Nodes
it contains NodeBase::NodeAudioProcessor and/or NodeBase::NodeDataProcessor
	

*/
#ifndef NODEBASE_H_INCLUDED
#define NODEBASE_H_INCLUDED

#include <JuceHeader.h>
#include "DataProcessor.h"

class NodeBaseUI;
class NodeManager;


class NodeBase : public ReferenceCountedObject, public DataProcessor::Listener
{

public:
	class NodeAudioProcessor : public juce::AudioProcessor,public AsyncUpdater
	{
	public:
		NodeAudioProcessor() :AudioProcessor(){

        };
		
		virtual const String getName() const override { return "NodeBaseProcessor"; };

        virtual void prepareToPlay(double sampleRate,int estimatedSamplesPerBlock) override
            {};
        virtual void releaseResources() override {};

        

		bool silenceInProducesSilenceOut() const override { return false; }

        virtual AudioProcessorEditor* createEditor() override {return nullptr ;}
        virtual bool hasEditor() const override { return false; }



		// dumb overrides from JUCE AudioProcessor :  MIDI
		int getNumPrograms() override { return 0; }
		int getCurrentProgram() override { return 0; }
		void setCurrentProgram(int index) override {}
		const String getProgramName(int index) override { return "NoProgram"; }
		void changeProgramName(int index, const String& newName) override {};
		double getTailLengthSeconds() const override { return 0; }
		bool acceptsMidi() const override { return false; }
		bool producesMidi() const override { return false; }


		// save procedures from host
		virtual void getStateInformation(juce::MemoryBlock& destData) override {};
		virtual void setStateInformation(const void* data, int sizeInBytes) override {};


//        AudioProcessor * getAudioProcessor()const {return audioProcessorImpl;};
		virtual void processBlock(AudioBuffer<float>& buffer,
                                  MidiBuffer& midiMessages) override {
            processBlockInternal(buffer, midiMessages);
            if(listeners.size()){
                updateRMS(buffer);
                triggerAsyncUpdate();
            }
            
            
        };
        
        virtual void processBlockInternal(AudioBuffer<float>& buffer,
                                          MidiBuffer& midiMessages) = 0;
        
        
        float updateRMS(AudioBuffer<float>& buffer){
            rmsValue = alphaRMS * buffer.getRMSLevel(0, 0, buffer.getNumSamples()) + (1-alphaRMS) * rmsValue;
        }
        float alphaRMS = 0.5;
        float rmsValue = 0;

        
        //Listener are called from non audio thread
        void handleAsyncUpdate() override{
            listeners.call(&Listener::RMSChanged,rmsValue);
        }
        
        class  Listener : public AsyncUpdater
        {
        public:
            /** Destructor. */
            virtual ~Listener() {}
            virtual void RMSChanged(float ) = 0;
           
        };
        
        ListenerList<Listener> listeners;
        void addListener(Listener* newListener) { listeners.add(newListener); }
        void removeListener(Listener* listener) { listeners.remove(listener); }
        
        ScopedPointer<AudioProcessor> audioProcessorImpl;
        
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeAudioProcessor)
	};


	class NodeDataProcessor : public DataProcessor
	{
	public:
		NodeDataProcessor() {};
		
		// Inherited via DataProcessor
		virtual void receiveData(const Data * incomingData, const String &destDataName, const String &destElementName = "", const String &sourceElementName = "") override;
		virtual void sendData(const Data * outgoingData, const String &sourceElementName = "") override;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeDataProcessor)

	};


	
public:
	NodeBase(NodeManager * nodeManager,uint32 nodeId, String name = "[NodeBase]", NodeBase::NodeAudioProcessor * audioProcessor = nullptr, NodeBase::NodeDataProcessor * dataProcessor = nullptr);
	virtual ~NodeBase();

	uint32 nodeId;
	String name;
    NodeManager * nodeManager;
    
	// owned by audio Graph in a refference Counted Array
    NodeAudioProcessor *  audioProcessor;
	//owned by data Graph in a reference counted array
	NodeDataProcessor * dataProcessor;

	bool hasAudioInputs;
	bool hasAudioOutputs;
	bool hasDataInputs;
	bool hasDataOutputs;
	
	void checkInputsAndOutputs();

	void remove();
	
    //audio
    
    
    void addToAudioGraphIfNeeded();
    void removeFromAudioGraphIfNeeded();
	//ui
	virtual NodeBaseUI *  createUI() { 
		DBG("No implementation in child node class !");
		jassert(false);
		return nullptr;
	}
	
	// Inherited via DataProcessor::Listener
	virtual void inputAdded(DataProcessor::Data *) override;
	virtual void inputRemoved(DataProcessor::Data *) override;
	virtual void outputAdded(DataProcessor::Data *) override;
	virtual void ouputRemoved(DataProcessor::Data *) override;



	//Listener
	class Listener
	{
	public:
		virtual ~Listener() {}
		virtual void askForRemoveNode(NodeBase *) = 0;

	};

	ListenerList<Listener> listeners;
	void addListener(Listener* newListener) { listeners.add(newListener); }
	void removeListener(Listener* listener) { listeners.remove(listener); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeBase)

		
};

#endif  // NODEBASE_H_INCLUDED
