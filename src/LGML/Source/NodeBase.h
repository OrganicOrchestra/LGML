/*
 ==============================================================================

 NodeBase.h
 Created: 2 Mar 2016 8:36:17pm
 Author:  bkupe

 ==============================================================================
 */

/*
 NodeBase is the base class for all Nodes
 it contains NodeAudioProcessor and/or NodeBase::NodeDataProcessor


 */
#ifndef NODEBASE_H_INCLUDED
#define NODEBASE_H_INCLUDED



#include "Data.h"
#include "ControllableContainer.h"
#include "PresetManager.h"


class NodeBaseUI;
class NodeManager;


class NodeBase :
	public ReferenceCountedObject,
	public ControllableContainer,
	public juce::AudioProcessor, public AsyncUpdater, //Audio
	public Data::DataListener //Data
{

public:
	NodeBase(NodeManager * nodeManager, uint32 nodeId, const String &name = "[NodeBase]");
	virtual ~NodeBase();

	uint32 nodeId;
	NodeManager * nodeManager;

	bool hasAudioInputs();
	bool hasAudioOutputs();
	bool hasDataInputs();
	bool hasDataOutputs();

	void remove(bool askBeforeRemove = false);


	//Controllables (from ControllableContainer)
	StringParameter * nameParam;
	BoolParameter * enabledParam;
	FloatParameter * xPosition;
	FloatParameter * yPosition;


	//audio
	void addToAudioGraphIfNeeded();
	void removeFromAudioGraphIfNeeded();


	virtual String getPresetFilter() override;

	//ui
	virtual NodeBaseUI *  createUI() {
		DBG("No implementation in child node class !");
		jassert(false);
		return nullptr;
	}

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;


public:

	//Listener
	class NodeListener
	{
	public:
		virtual ~NodeListener() {}
		virtual void askForRemoveNode(NodeBase *) {}

		virtual void nodeEnableChanged(NodeBase *) {}
	};

	ListenerList<NodeListener> nodeListeners;
	void addNodeListener(NodeListener* newListener) { nodeListeners.add(newListener); }
	void removeNodeListener(NodeListener* listener) { nodeListeners.remove(listener); }


	virtual const String getName() const override
	{
		return niceName;
	}


private:

	int nodeTypeUID;
	friend class NodeFactory;

	void parameterValueChanged(Parameter * p) override;





	//AUDIO PROCESSOR
public:
	FloatParameter * outputVolume;
	BoolParameter * bypass;
	StringArray inputChannelNames;
	StringArray outputChannelNames;

	bool setPreferedNumAudioInput(int num);
	bool setPreferedNumAudioOutput(int num);

	void setInputChannelNames(int startChannel, StringArray names);
	void setOutputChannelNames(int startChannel, StringArray names);
	void setInputChannelName(int channelIndex, const String &name);
	void setOutputChannelName(int channelIndex, const String &name);
	String getInputChannelName(int channelIndex);
	String getOutputChannelName(int channelIndex);


	virtual void prepareToPlay(double, int) override {};
	virtual void releaseResources() override {};

	//bool silenceInProducesSilenceOut() const override { return false; }

	virtual AudioProcessorEditor* createEditor() override { return nullptr; }
	virtual bool hasEditor() const override { return false; }



	// dumb overrides from JUCE AudioProcessor :  MIDI
	int getNumPrograms() override { return 0; }
	int getCurrentProgram() override { return 0; }
	void setCurrentProgram(int) override {}
	const String getProgramName(int) override { return "NoProgram"; }
	void changeProgramName(int, const String&) override {};
	double getTailLengthSeconds() const override { return 0; }
	bool acceptsMidi() const override { return false; }
	bool producesMidi() const override { return false; }
	void numChannelsChanged()override {}


	// save procedures from host
	virtual void getStateInformation(juce::MemoryBlock&) override {};
	virtual void setStateInformation(const void*, int) override {};


	virtual void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
	virtual void processBlockInternal(AudioBuffer<float>& /*buffer*/ , MidiBuffer& /*midiMessage*/ ) {};

	//RMS
	void updateRMS(const AudioBuffer<float>& buffer, float &targetRMSValue);

	float alphaRMSIn = 0.05f;
	float rmsValueIn = 0.f;
	const int samplesBeforeRMSInUpdate = 512;
	int curSamplesForRMSInUpdate = 0;
	float alphaRMSOut = 0.05f;
	float rmsValueOut = 0.f;
	const int samplesBeforeRMSOutUpdate = 512;
	int curSamplesForRMSOutUpdate = 0;

	//Listener are called from non audio thread
	void handleAsyncUpdate() override {
		rmsListeners.call(&RMSListener::RMSChanged, rmsValueIn, rmsValueOut);
	}

	class  RMSListener
	{
	public:
		/** Destructor. */
		virtual ~RMSListener() {}
		virtual void RMSChanged(float rmsInValue, float rmsOutValue) = 0;

	};

	ListenerList<RMSListener> rmsListeners;
	void addRMSListener(RMSListener* newListener) { rmsListeners.add(newListener); }
	void removeRMSListener(RMSListener* listener) { rmsListeners.remove(listener); }

	class NodeAudioProcessorListener {
	public:
		virtual ~NodeAudioProcessorListener() {};
		virtual void numAudioInputChanged(int) {};
		virtual void numAudioOutputChanged(int) {};
	};

	ListenerList<NodeAudioProcessorListener> nodeAudioProcessorListeners;
	void addNodeAudioProcessorListener(NodeAudioProcessorListener* newListener) { nodeAudioProcessorListeners.add(newListener); }
	void removeNodeAudioProcessorListener(NodeAudioProcessorListener* listener) { nodeAudioProcessorListeners.remove(listener); }


	bool wasSuspended;
	float lastVolume;






	//DATA
public:
	typedef Data::DataType DataType;
	typedef Data::DataElement DataElement;

	OwnedArray<Data> inputDatas;
	OwnedArray<Data> outputDatas;
	bool enabled;

	Data * addInputData(const String &name, DataType type);
	Data * addOutputData(const String &name, DataType type);

	void removeInputData(const String &name);
	void removeOutputData(const String &name);

	void inputDataChanged(Data *)
	{
		//to be overriden by child classes
	}

	virtual void updateOutputData(String &dataName, const float &value1, const float &value2 = 0, const float &value3 = 0);


	int getTotalNumInputData() const { return inputDatas.size(); }
	int getTotalNumOutputData() const { return outputDatas.size(); }

	StringArray getInputDataInfos();

	StringArray getOutputDataInfos();

	Data::DataType getInputDataType(const String &dataName, const String &elementName);
	Data::DataType getOutputDataType(const String &dataName, const String &elementName);


	Data * getOutputDataByName(const String &dataName);
	Data * getInputDataByName(const String &dataName);

	//Listener
	class  NodeDataProcessorListener
	{
	public:
		/** Destructor. */
		virtual ~NodeDataProcessorListener() {}

		virtual void inputAdded(Data *) {}
		virtual void inputRemoved(Data *) {}

		virtual void outputAdded(Data *) {}
		virtual void ouputRemoved(Data *) {}

		virtual void inputDataChanged(Data *) {}
	};

	ListenerList<NodeDataProcessorListener> dataProcessorListeners;
	void addDataProcessorListener(NodeDataProcessorListener* newListener) { dataProcessorListeners.add(newListener); }
	void removeDataProcessorListener(NodeDataProcessorListener* listener) { dataProcessorListeners.remove(listener); }


	private:
		virtual void dataChanged(Data *) override;








    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeBase)

};

#endif  // NODEBASE_H_INCLUDED
