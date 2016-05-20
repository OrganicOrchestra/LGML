/*
 ==============================================================================

 NodeBase.cpp
 Created: 2 Mar 2016 8:36:17pm
 Author:  bkupe

 ==============================================================================
 */

#include "NodeBase.h"
#include "NodeManager.h"
#include "TimeManager.h"


NodeBase::NodeBase(const String &name,NodeType _type, bool _hasMainAudioControl) :
	ConnectableNode(name,_type,_hasMainAudioControl),
	audioNode(nullptr)
{
    addToAudioGraph();
	lastVolume = hasMainAudioControl ? outputVolume->floatValue() : 0;

}


NodeBase::~NodeBase()
{
	clear();
}


bool NodeBase::hasAudioInputs()
{
	//to override
	return getTotalNumInputChannels() > 0;
}

bool NodeBase::hasAudioOutputs()
{
	//to override
	return getTotalNumOutputChannels() > 0;
}

bool NodeBase::hasDataInputs()
{
	//to override
	return getTotalNumInputData()>0;
}

bool NodeBase::hasDataOutputs()
{
	//to override
	return getTotalNumOutputData()>0;
}


void NodeBase::parameterValueChanged(Parameter * p)
{
	ConnectableNode::parameterValueChanged(p);

	if (p == enabledParam)
	{
		suspendProcessing(!enabledParam->boolValue());
	}
}

void NodeBase::clear()
{
	// get called after deletion of TimeManager on app exit
	TimeManager * tm = TimeManager::getInstanceWithoutCreating();
	if (tm != nullptr)
	{
		tm->releaseMasterNode(this);
	}

	//Data
	inputDatas.clear();
	outputDatas.clear();

	//removeFromAudioGraph();
}



String NodeBase::getPresetFilter()
{
    return NodeFactory::nodeToString(this);
}


//Save / Load

var NodeBase::getJSONData()
{
	var data = ConnectableNode::getJSONData();
  
   MemoryBlock m;

    // TODO we could implement that for all node objects to be able to save any kind of custom data
    getStateInformation(m);

    if (m.getSize()) {
        var audioProcessorData(new DynamicObject());
        audioProcessorData.getDynamicObject()->setProperty("state", m.toBase64Encoding());
        data.getDynamicObject()->setProperty("audioProcessor", audioProcessorData);
    }

    return data;
}

void NodeBase::loadJSONDataInternal(var data)
{
	ConnectableNode::loadJSONDataInternal(data);

    var audioProcessorData = data.getProperty("audioProcessor", var());
    String audioProcessorStateData = audioProcessorData.getProperty("state",var());

    MemoryBlock m;
    m.fromBase64Encoding(audioProcessorStateData);
    setStateInformation(m.getData(), (int)m.getSize());
}

//ui

ConnectableNodeUI * NodeBase::createUI() {
	DBG("No implementation in child node class !");
	jassert(false);
	return nullptr;
}




/////////////////////////////////////// AUDIO

AudioProcessorGraph::Node * NodeBase::getAudioNode(bool)
{
	return audioNode;
}

void NodeBase::addToAudioGraph() {
	if (NodeManager::getInstanceWithoutCreating() != nullptr)
	{
		audioNode = NodeManager::getInstance()->audioGraph.addNode(this);
	}
}

void NodeBase::removeFromAudioGraph() {
	if (NodeManager::getInstanceWithoutCreating() != nullptr)
	{
		if (audioNode != nullptr) NodeManager::getInstance()->audioGraph.removeNode(audioNode);
	}
	
}

void NodeBase::processBlock(AudioBuffer<float>& buffer,
	MidiBuffer& midiMessages) {

	bool doUpdateRMSIn = false;
	bool doUpdateRMSOut = false;

	if (rmsListeners.size()) {
		updateRMS(buffer, rmsValueIn);
		curSamplesForRMSInUpdate += buffer.getNumSamples();

		if (curSamplesForRMSInUpdate >= samplesBeforeRMSInUpdate) {
			doUpdateRMSIn = true;
			curSamplesForRMSInUpdate = 0;
		}
	}


	if (!isSuspended())
	{
		if (!hasMainAudioControl || (hasMainAudioControl && !bypass->boolValue()) ){
			processBlockInternal(buffer, midiMessages);
            if(hasMainAudioControl){
                buffer.applyGainRamp(0, buffer.getNumSamples(), lastVolume, outputVolume->floatValue());
                lastVolume = outputVolume->floatValue();
            }

			if (wasSuspended) {
				buffer.applyGainRamp(0, buffer.getNumSamples(), 0, 1);
				wasSuspended = false;
			}
		}

		if (rmsListeners.size()) {
			updateRMS(buffer, rmsValueOut);
			curSamplesForRMSOutUpdate += buffer.getNumSamples();

			if (curSamplesForRMSOutUpdate >= samplesBeforeRMSOutUpdate) {
				doUpdateRMSOut = true;
				curSamplesForRMSOutUpdate = 0;
			}
		}

		if (doUpdateRMSIn || doUpdateRMSOut) triggerAsyncUpdate();

	}

	else {
		if (!wasSuspended) {
			buffer.applyGainRamp(0, buffer.getNumSamples(), 1, 0); wasSuspended = true;
		}
		else {
			buffer.clear();
		}
	}

};

bool NodeBase::setPreferedNumAudioInput(int num) {
	setPlayConfigDetails(num, getTotalNumOutputChannels(),
		getSampleRate(),
		getBlockSize());

	if (NodeManager::getInstanceWithoutCreating() != nullptr)
	{
		NodeManager::getInstance()->audioGraph.prepareToPlay(NodeManager::getInstance()->audioGraph.getBlockSize(), (int)NodeManager::getInstance()->audioGraph.getSampleRate());
	}

	nodeAudioProcessorListeners.call(&NodeAudioProcessorListener::numAudioInputChanged, this,num);

	return true;
}
bool NodeBase::setPreferedNumAudioOutput(int num) {
	setPlayConfigDetails(getTotalNumInputChannels(), num,
		getSampleRate(),
		getBlockSize());
	if (NodeManager::getInstanceWithoutCreating() != nullptr)
	{
		NodeManager::getInstance()->audioGraph.prepareToPlay(NodeManager::getInstance()->audioGraph.getBlockSize(), (int)NodeManager::getInstance()->audioGraph.getSampleRate());
	}
	nodeAudioProcessorListeners.call(&NodeAudioProcessorListener::numAudioOutputChanged,this,num);
	return true;
}

void NodeBase::updateRMS(const AudioBuffer<float>& buffer, float &targetRmsValue) {
	int numSamples = buffer.getNumSamples();
	int numChannels = buffer.getNumChannels();

#ifdef HIGH_ACCURACY_RMS
	for (int i = numSamples - 64; i >= 0; i -= 64) {
		rmsValue += alphaRMS * (buffer.getRMSLevel(0, i, 64) - rmsValue);
	}
#else
	// faster implementation taken from juce Device Settings input meter
	for (int j = 0; j <numSamples; ++j)
	{
		float s = 0;
		for (int i = numChannels - 1; i >0; --i)
			s = jmax(s, std::abs(buffer.getSample(i, j)));


		const double decayFactor = 0.99992;
		if (s > targetRmsValue)
			targetRmsValue = s;
		else if (targetRmsValue > 0.001f)
			targetRmsValue *= (float)decayFactor;
		else
			targetRmsValue = 0;
	}
#endif


}


void NodeBase::handleAsyncUpdate()
{
	rmsListeners.call(&RMSListener::RMSChanged, this, rmsValueIn, rmsValueOut);
}

//////////////////////////////////   DATA

Data * NodeBase::getInputData(int dataIndex)
{
	return inputDatas[dataIndex];
}


Data * NodeBase::getOutputData(int dataIndex)
{
	return outputDatas[dataIndex];
}


Data * NodeBase::addInputData(const String & name, Data::DataType dataType)
{
	Data *d = new Data(this, name, dataType);
	inputDatas.add(d);

	d->addDataListener(this);

	return d;
}

Data * NodeBase::addOutputData(const String & name, DataType dataType)
{
	Data * d = new Data(this, name, dataType);
	outputDatas.add(d);

	return d;
}

void NodeBase::removeInputData(const String & name)
{
	Data * d = getInputDataByName(name);
	if (d == nullptr) return;

	inputDatas.removeObject(d, true);
}

void NodeBase::removeOutputData(const String & name)
{
	Data * d = getOutputDataByName(name);
	if (d == nullptr) return;
	outputDatas.removeObject(d, true);
}

void NodeBase::updateOutputData(String & dataName, const float & value1, const float & value2, const float & value3)
{
	Data * d = getOutputDataByName(dataName);
	if (d != nullptr) d->update(value1, value2, value3);
}

int NodeBase::getTotalNumInputData() { 
	return inputDatas.size(); 
}

int NodeBase::getTotalNumOutputData() { 
	return outputDatas.size(); 
}

StringArray NodeBase::getInputDataInfos()
{
	StringArray dataInfos;
	for (auto &d : inputDatas) dataInfos.add(d->name + " (" + d->getTypeString() + ")");
	return dataInfos;
}

StringArray NodeBase::getOutputDataInfos()
{
	StringArray dataInfos;
	for (auto &d : outputDatas) dataInfos.add(d->name + " (" + d->getTypeString() + ")");
	return dataInfos;
}

Data::DataType NodeBase::getInputDataType(const String &dataName, const String &elementName)
{
	for (int i = inputDatas.size(); --i >= 0;)
	{
		Data* d = inputDatas.getUnchecked(i);

		if (d->name == dataName)
		{
			if (elementName.isEmpty())
			{
				return d->type;
			}
			else
			{
				DataElement * e = d->getElement(elementName);
				if (e == nullptr) return DataType::Unknown;
				return e->type;
			}
		}
	}

	return DataType::Unknown;
}

Data::DataType NodeBase::getOutputDataType(const String &dataName, const String &elementName)
{
	for (int i = outputDatas.size(); --i >= 0;)
	{
		Data* d = outputDatas.getUnchecked(i);

		if (d->name == dataName)
		{
			if (elementName.isEmpty())
			{
				return d->type;
			}
			else
			{
				DataElement * e = d->getElement(elementName);
				if (e == nullptr) return DataType::Unknown;
				return e->type;
			}
		}
	}

	return DataType::Unknown;

}

Data * NodeBase::getOutputDataByName(const String & dataName)
{
	for (auto &d : outputDatas)
	{
		if (d->name == dataName) return d;
	}

	return nullptr;
}

Data * NodeBase::getInputDataByName(const String & dataName)
{
	for (auto &d : inputDatas)
	{
		if (d->name == dataName) return d;
	}

	return nullptr;
}

void NodeBase::dataChanged(Data * d)
{
	if (enabled) {
		processInputDataChanged(d);
	}
}

void NodeBase::processInputDataChanged(Data *)
{
}
