/*
 ==============================================================================

 VSTNode.cpp
 Created: 2 Mar 2016 8:37:24pm
 Author:  bkupe

 ==============================================================================
 */

#include "VSTNode.h"
#include "VSTNodeUI.h"
#include "NodeManager.h"

AudioDeviceManager& getAudioDeviceManager();


VSTNode::VSTNode() :
	NodeBase("VST",NodeType::VSTType),
	blockFeedback(false)
{
    identifierString = addStringParameter("VST Identifier","string that identify a VST","");
    addChildControllableContainer(&pluginWindowParameter);

	midiActivityTrigger = addTrigger("Midi Activity", "Midi Activity indicator");
	midiPortNameParam = addStringParameter("midiPortName", "MIDI Port Name", "");
	midiPortNameParam->hideInEditor = true;
}


VSTNode::~VSTNode(){
    PluginWindow::closeCurrentlyOpenWindowsFor (this);
}


void  VSTNode::createPluginWindow(){
    if (PluginWindow* const w = PluginWindow::getWindowFor (this))
        w->toFront (true);
}

void VSTNode::closePluginWindow(){
    PluginWindow::closeCurrentlyOpenWindowsFor (this);
}

void VSTNode::onContainerParameterChanged(Parameter * p) {
    NodeBase::onContainerParameterChanged(p);
    if(p==identifierString){
        if(identifierString->value!=""){
            PluginDescription * pd = VSTManager::getInstance()->knownPluginList.getTypeForIdentifierString (identifierString->value);
            if(pd){
                generatePluginFromDescription(pd);
            }
            else{DBG("VST : cant find plugin for identifier : "+identifierString->value.toString());}
        }
        else{DBG("VST : no identifierString provided");}
	}if (p == midiPortNameParam)
	{
		setCurrentDevice(midiPortNameParam->stringValue());
		vstNodeListeners.call(&VSTNodeListener::midiDeviceChanged);
	}

    // a VSTParameter is changed
    else{
        if(blockFeedback)return;
        for(int i = VSTParameters.size() -1; i>=0;--i){
            if(VSTParameters.getUnchecked(i) == p){
                innerPlugin->setParameter(i, VSTParameters.getUnchecked(i)->value);
                break;
            }

        }
    }
};
void VSTNode::initParametersFromProcessor(AudioProcessor * p){

    p->addListener(this);

    for(auto &c:VSTParameters){
        removeControllable(c);
    }

    VSTParameters.clear();

    for(int i = 0 ; i < p->getNumParameters() ; i++){
        VSTParameters.add(addFloatParameter(p->getParameterName(i), p->getParameterLabel(i), p->getParameter(i)));
    }

    vstNodeListeners.call(&VSTNodeListener::newVSTSelected);
}

/*
String VSTNode::getPresetFilter()
{
    return identifierString->stringValue();
}
*/


void VSTNode::generatePluginFromDescription(PluginDescription * desc)
{

	innerPlugin = nullptr;
	String errorMessage;
	AudioDeviceManager::AudioDeviceSetup result;

	// set max channels to this
	// TODO check that it actually works
	desc->numInputChannels = jmin(desc->numInputChannels, getMainBusNumInputChannels());
	desc->numOutputChannels = jmin(desc->numOutputChannels, getMainBusNumOutputChannels());


	getAudioDeviceManager().getAudioDeviceSetup(result);

	if (AudioPluginInstance* instance = VSTManager::getInstance()->formatManager.createPluginInstance
		(*desc, result.sampleRate, result.bufferSize, errorMessage)) {
		// try to align the precision of the processor and the graph

		instance->setPreferredBusArrangement(true, 0, AudioChannelSet::canonicalChannelSet(getMainBusNumInputChannels()));
		instance->setPreferredBusArrangement(false, 0, AudioChannelSet::canonicalChannelSet(getMainBusNumOutputChannels()));
		int numIn = instance->getMainBusNumInputChannels();
		int numOut = instance->getMainBusNumOutputChannels();
		setPlayConfigDetails(numIn, numOut, result.sampleRate, result.bufferSize);

		//@Martin i addedd this because when not playing, it crashed
        // @ben it is necessary
        // if it triggers an assert it's that vst is wrongly implemened (and there are a lot...)
        // ignoring the assert seems fair enough for now (juce_VSTPluginFormat.cpp l:794 while checking doubleprecision)

            instance->setProcessingPrecision(singlePrecision);
            instance->prepareToPlay(result.sampleRate, result.bufferSize);

        instance->setPlayHead(getPlayHead());
        innerPlugin = instance;
        messageCollector.reset (getSampleRate());
		initParametersFromProcessor(instance);
	}

	else {

		DBG(errorMessage);
		jassertfalse;
	}
}

void VSTNode::numChannelsChanged(){
	if (NodeManager::getInstanceWithoutCreating() != nullptr)
	{
		NodeManager::getInstance()->audioGraph.removeIllegalConnections();
		// hack to force update renderingops in audioGraph
		NodeManager::getInstance()->audioGraph.removeConnection(-1);
	}
}

inline void VSTNode::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer & ) {
	if (innerPlugin) {
		if (buffer.getNumChannels() >= jmax(innerPlugin->getTotalNumInputChannels(), innerPlugin->getTotalNumOutputChannels()))
		{
            incomingMidi.clear();
            messageCollector.removeNextBlockOfMessages (incomingMidi, buffer.getNumSamples());
            innerPlugin->setPlayHead(NodeBase::getPlayHead());
			innerPlugin->processBlock(buffer, incomingMidi);
		}
		else {
			static int numFrameDropped = 0;
			DBG("dropAudio " + String(numFrameDropped++));
		}
	}
}


void VSTNode::audioProcessorParameterChanged (AudioProcessor* ,
                                              int parameterIndex,
                                              float newValue) {

    jassert(parameterIndex<VSTParameters.size());
    blockFeedback = true;
    VSTParameters.getUnchecked(parameterIndex)->setValue(newValue);
    blockFeedback = false;
}




ConnectableNodeUI * VSTNode::createUI() {
	return new NodeBaseUI(this, new VSTNodeContentUI, new VSTNodeHeaderUI);
}


void VSTNode::setCurrentDevice(const String & deviceName)
{
	MIDIListener::setCurrentDevice(deviceName);
	midiPortNameParam->setValue(deviceName, true);
}

void VSTNode::handleIncomingMidiMessage(MidiInput* ,
                               const MidiMessage& message) {
    if (innerPlugin)
        messageCollector.addMessageToQueue (message);

	midiActivityTrigger->trigger();
};

void VSTNode::getStateInformation(MemoryBlock & destData) {
    if (innerPlugin) {
        innerPlugin->getStateInformation(destData);
    };
}

void VSTNode::setStateInformation(const void* data, int sizeInBytes) {
    if (innerPlugin) {
        innerPlugin->setStateInformation(data, sizeInBytes); };
};

void VSTNode::loadPresetInternal(PresetManager::Preset * preset){
    var v = preset->getPresetValue("/rawData");
    jassert(v.isUndefined() || v.isString());
    MemoryBlock m;
    m.fromBase64Encoding(v.toString());
    setStateInformation(m.getData(),(int)m.getSize());


};
void VSTNode::savePresetInternal(PresetManager::Preset * preset){

    MemoryBlock m;
    getStateInformation(m);
    preset->addPresetValue("/rawData",var(m.toBase64Encoding()));

};
