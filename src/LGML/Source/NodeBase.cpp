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


NodeBase::NodeBase(NodeManager * nodeManager,uint32 _nodeId, const String &name) :
nodeManager(nodeManager),
nodeId(_nodeId),
ControllableContainer(name),
nodeTypeUID(0) // UNKNOWNTYPE
{

    addToAudioGraphIfNeeded();

    //set Params
    nameParam = addStringParameter("Name", "Set the name of the node.", name);
    enabledParam = addBoolParameter("Enabled", "Set whether the node is enabled or disabled", true);
    xPosition = addFloatParameter("xPosition","x position on canvas",0,0,99999);
    yPosition= addFloatParameter("yPosition","y position on canvas",0,0,99999);

    xPosition->isControllableExposed = false;
    yPosition->isControllableExposed = false;
    xPosition->isPresettable = false;
    yPosition->isPresettable = false;
    nameParam->isPresettable = false;
    enabledParam->isPresettable = false;


	//Audio
	outputVolume = addFloatParameter("masterVolume", "mester volume for this node", 1.);
	lastVolume = outputVolume->floatValue();
	bypass = addBoolParameter("Bypass", "by-pass current node, letting audio pass thru", false);


	setInputChannelName(0, "Main Left");
	setInputChannelName(1, "Main Right");
	setOutputChannelName(0, "Main Left");
	setOutputChannelName(1, "Main Right");


}


NodeBase::~NodeBase()
{
    // get called after deletion of TimeManager on app exit
    TimeManager * tm = TimeManager::getInstanceWithoutCreating();
    if(tm!=nullptr)
        tm->releaseMasterNode(this);


	//Data
	inputDatas.clear();
	outputDatas.clear();

	removeFromAudioGraphIfNeeded();
}

bool NodeBase::hasAudioInputs()
{
	return getTotalNumInputChannels() > 0;
}

bool NodeBase::hasAudioOutputs()
{
	return getTotalNumOutputChannels() > 0;
}

bool NodeBase::hasDataInputs()
{
	return getTotalNumInputData()>0;
}

bool NodeBase::hasDataOutputs()
{
	return getTotalNumOutputData()>0;
}

void NodeBase::remove(bool askBeforeRemove)
{
    if (askBeforeRemove)
    {
        int result = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::QuestionIcon, "Remove node", "Do you want to remove the node ?");
        if (result == 0) return;
    }

    nodeListeners.call(&NodeBase::NodeListener::askForRemoveNode,this);
}

void NodeBase::parameterValueChanged(Parameter * p)
{
    if (p == nameParam)
    {
        setNiceName(nameParam->stringValue());
    }else if (p == enabledParam)
	{
        suspendProcessing(!enabledParam->boolValue());
		DBG("Node Enabled changed !");
		nodeListeners.call(&NodeListener::nodeEnableChanged, this);
    }
    else{
          ControllableContainer::parameterValueChanged(p);
    }
}

void NodeBase::addToAudioGraphIfNeeded(){
    if(hasAudioInputs() || hasAudioOutputs()){
        nodeManager->audioGraph.addNode(this,nodeId);
	}
}

void NodeBase::removeFromAudioGraphIfNeeded(){
    if(hasAudioInputs() || hasAudioOutputs()){
        nodeManager->audioGraph.removeNode(nodeId);
    }
}

String NodeBase::getPresetFilter()
{
    return NodeFactory::nodeToString(this);
}


//Save / Load

var NodeBase::getJSONData()
{
	var data = ControllableContainer::getJSONData();
    data.getDynamicObject()->setProperty("nodeType", NodeFactory::nodeToString(this));
    data.getDynamicObject()->setProperty("nodeId", String(nodeId));

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
    var audioProcessorData = data.getProperty("audioProcessor", var());
    String audioProcessorStateData = audioProcessorData.getProperty("state",var());

    MemoryBlock m;
    m.fromBase64Encoding(audioProcessorStateData);
    setStateInformation(m.getData(), (int)m.getSize());
}




/////////////////////////////////////// AUDIO

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
		if (!bypass->boolValue()) {
			processBlockInternal(buffer, midiMessages);
			buffer.applyGainRamp(0, buffer.getNumSamples(), lastVolume, outputVolume->floatValue());
			lastVolume = outputVolume->floatValue();

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

	nodeManager->audioGraph.prepareToPlay(nodeManager->audioGraph.getBlockSize(), (int)nodeManager->audioGraph.getSampleRate());
	nodeAudioProcessorListeners.call(&NodeAudioProcessorListener::numAudioInputChanged, num);

	return true;
}
bool NodeBase::setPreferedNumAudioOutput(int num) {
	setPlayConfigDetails(getTotalNumInputChannels(), num,
		getSampleRate(),
		getBlockSize());

	nodeManager->audioGraph.prepareToPlay(nodeManager->audioGraph.getBlockSize(), (int)nodeManager->audioGraph.getSampleRate());
	nodeAudioProcessorListeners.call(&NodeAudioProcessorListener::numAudioOutputChanged, num);
	return true;
}

void NodeBase::setInputChannelNames(int startChannel, StringArray names)
{
	for (int i = startChannel; i < startChannel + names.size(); i++)
	{
		setInputChannelName(i, names[i]);
	}
}

void NodeBase::setOutputChannelNames(int startChannel, StringArray names)
{
	for (int i = startChannel; i < startChannel + names.size(); i++)
	{
		setOutputChannelName(i, names[i]);
	}
}

void NodeBase::setInputChannelName(int channelIndex, const String & name)
{
	while (inputChannelNames.size() < (channelIndex + 1))
	{
		inputChannelNames.add(String::empty);
	}

	inputChannelNames.set(channelIndex, name);
}

void NodeBase::setOutputChannelName(int channelIndex, const String & name)
{
	while (outputChannelNames.size() < (channelIndex + 1))
	{
		outputChannelNames.add(String::empty);
	}

	outputChannelNames.set(channelIndex, name);
}

String NodeBase::getInputChannelName(int channelIndex)
{
	String defaultName = "Input " + String(channelIndex);
	if (channelIndex < 0 || channelIndex >= inputChannelNames.size()) return defaultName;

	String s = inputChannelNames[channelIndex];
	if (s.isNotEmpty()) return s;
	return defaultName;
}

String NodeBase::getOutputChannelName(int channelIndex)
{
	String defaultName = "Output " + String(channelIndex);
	if (channelIndex < 0 || channelIndex >= outputChannelNames.size()) return defaultName;

	String s = outputChannelNames[channelIndex];
	if (s.isNotEmpty()) return s;
	return defaultName;
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




//////////////////////////////////   DATA


Data * NodeBase::addInputData(const String & name, Data::DataType type)
{
	Data *d = new Data(this, name, type);
	inputDatas.add(d);

	d->addDataListener(this);

	dataProcessorListeners.call(&NodeDataProcessorListener::inputAdded, d);

	return d;
}

Data * NodeBase::addOutputData(const String & name, DataType type)
{
	Data * d = new Data(this, name, type);
	outputDatas.add(d);

	dataProcessorListeners.call(&NodeDataProcessorListener::outputAdded, d);

	return d;
}

void NodeBase::removeInputData(const String & name)
{
	Data * d = getInputDataByName(name);
	if (d == nullptr) return;

	dataProcessorListeners.call(&NodeDataProcessorListener::inputRemoved, d);
	inputDatas.removeObject(d, true);
}

void NodeBase::removeOutputData(const String & name)
{
	Data * d = getOutputDataByName(name);
	if (d == nullptr) return;
	dataProcessorListeners.call(&NodeDataProcessorListener::ouputRemoved, d);
	outputDatas.removeObject(d, true);
}

void NodeBase::updateOutputData(String & dataName, const float & value1, const float & value2, const float & value3)
{
	Data * d = getOutputDataByName(dataName);
	if (d != nullptr) d->update(value1, value2, value3);
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
		dataProcessorListeners.call(&NodeDataProcessorListener::inputDataChanged, d);
	}
}
