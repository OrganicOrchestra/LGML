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



#include "ControllableContainer.h"
#include "PresetManager.h"
#include "ConnectableNode.h"

class ConnectableNodeUI;
class NodeManager;


class NodeBase :
	public ConnectableNode,
	public ReferenceCountedObject,
	public juce::AudioProcessor, public AsyncUpdater, //Audio
	public Data::DataListener //Data
{

public:
	NodeBase(const String &name = "[NodeBase]", NodeType type = UNKNOWN_TYPE, bool _hasMainAudioControl = true);
	virtual ~NodeBase();


	virtual bool hasAudioInputs() override;
	virtual bool hasAudioOutputs() override;
	virtual bool hasDataInputs() override;
	virtual bool hasDataOutputs() override;


	virtual String getPresetFilter() override;

//  TODO:  this should not be implemented in Node to avoid overriding this method
//    create onNodeParameterChanged();
	void onContainerParameterChanged(Parameter * p) override;
public:


	virtual void clear() override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;


	//ui
	virtual ConnectableNodeUI *  createUI() override;;

	virtual const String getName() const override
	{
		return niceName;
	}

	class NodeBaseListener {
	public:
		virtual ~NodeBaseListener() {};
		virtual void numAudioInputChanged(NodeBase *, int /*newNumInput*/) {};
		virtual void numAudioOutputChanged(NodeBase *, int /*newNumOutput*/) {};
		virtual void numDataInputChanged(NodeBase *, int /*newNumInput*/) {};
		virtual void numDataOutputChanged(NodeBase *, int /*newNumOutput*/) {};

		virtual void audioInputAdded(NodeBase *, int /*channel*/) {}
		virtual void audioInputRemoved(NodeBase *, int /*channel*/) {}
		virtual void audioOutputAdded(NodeBase *, int /*channel*/) {}
		virtual void audioOutputRemoved(NodeBase *, int /*channel*/) {}


		virtual void dataInputAdded(NodeBase *, Data *) {}
		virtual void dataInputRemoved(NodeBase *, Data *) {}
		virtual void dataOutputAdded(NodeBase *, Data *) {}
		virtual void dataOutputRemoved(NodeBase *, Data *) {}

	};

	ListenerList<NodeBaseListener> nodeBaseListeners;
	void addNodeBaseListener(NodeBaseListener* newListener) { nodeBaseListeners.add(newListener); }
	void removeNodeBaseListener(NodeBaseListener* listener) { nodeBaseListeners.remove(listener); }

	//AUDIO PROCESSOR

	AudioProcessorGraph::Node * audioNode;



	virtual AudioProcessorGraph::Node * getAudioNode(bool isInputNode = true) override;
	void addToAudioGraph() override;
	void removeFromAudioGraph() override;

	bool setPreferedNumAudioInput(int num);
	bool setPreferedNumAudioOutput(int num);


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

	const float alphaRMSIn = 0.05f;
	float rmsValueIn = 0.f;
	const int samplesBeforeRMSInUpdate = 512;
	int curSamplesForRMSInUpdate = 0;
	const float alphaRMSOut = 0.05f;
	float rmsValueOut = 0.f;
	const int samplesBeforeRMSOutUpdate = 512;
	int curSamplesForRMSOutUpdate = 0;

	//Listener are called from non audio thread
	void handleAsyncUpdate() override;






	bool wasSuspended;
	float lastVolume;

	//DATA
	virtual Data* getInputData(int dataIndex) override;
	virtual Data* getOutputData(int dataIndex) override;


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


	int getTotalNumInputData() override;
	int getTotalNumOutputData() override;

	StringArray getInputDataInfos() override;
	StringArray getOutputDataInfos() override;

	Data::DataType getInputDataType(const String &dataName, const String &elementName) override;
	Data::DataType getOutputDataType(const String &dataName, const String &elementName) override;

	Data * getOutputDataByName(const String &dataName) override;
	Data * getInputDataByName(const String &dataName) override;

	virtual void dataChanged(Data *) override;
	virtual void processInputDataChanged(Data *);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeBase)

};

#endif  // NODEBASE_H_INCLUDED
