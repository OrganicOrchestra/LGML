/* Copyright © Organic Orchestra, 2017
 *
 * This file is part of LGML.  LGML is a software to manipulate sound in realtime
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation (version 3 of the License).
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */


#include "VSTNode.h"

#include "../Manager/NodeManager.h"
#include "../../Utils/DebugHelpers.h"
#include "../../Audio/VSTManager.h"
#include "../../Time/TimeManager.h"


constexpr int maxVSTParamNameSize = 100;

extern AudioDeviceManager& getAudioDeviceManager();

REGISTER_NODE_TYPE (VSTNode)
//class VSTLoaderJob : public ThreadPoolJob
//{
//
//    public :
//    VSTLoaderJob (PluginDescription* _pd, VSTNode* node): ThreadPoolJob ("VSTLoader : " + node->shortName), pd (_pd), originNode (node) {}
//    PluginDescription* pd;
//    VSTNode* originNodVSTLOADING_THREADEDe;
//
//    JobStatus runJob() override
//    {
//        originNode->generatePluginFromDescription (pd);
//        originNode->triggerAsyncUpdate();
//        return JobStatus::jobHasFinished;
//    }
//
//};


VSTNode::VSTNode (StringRef name) :
NodeBase (name),
blockFeedback (false),
midiChooser(this,false,true)
{
    identifierString = addNewParameter<StringParameter> ("VST Identifier", "string that identify a VST", "");
    identifierString->isEditable = false;
    identifierString->isHidenInEditor = true;

    addChildControllableContainer (&pluginWindowParameter);

    midiActivityTrigger =  addNewParameter<Trigger> ("Midi Activity", "Midi Activity indicator");
    midiActivityTrigger->isControllableExposed = false;
    //    midiPortNameParam = addNewParameter<StringParameter> ("midiPortName", "MIDI Port Name", "");
    //    midiPortNameParam->isHidenInEditor = true;

    processWhenBypassed = addNewParameter<BoolParameter> ("processWhenBypassed", "some effects (Reverbs ...) need to process constantly even when bypassed", false);
    bProcessWhenBypassed = processWhenBypassed->boolValue();
    setPreferedNumAudioInput (2);
    setPreferedNumAudioOutput (2);
}


VSTNode::~VSTNode()
{
    cancelPendingUpdate();

}



void VSTNode::processBlockBypassed (AudioBuffer<float>& buffer, MidiBuffer&)
{

    if (innerPlugin)
    {
        incomingMidi.clear();
        messageCollector.removeNextBlockOfMessages (incomingMidi, buffer.getNumSamples());
        innerPlugin->setPlayHead ((AudioPlayHead*)TimeManager::getInstance());

        if (bProcessWhenBypassed)
        {
            innerPlugin->processBlock (buffer, incomingMidi);
        }
    }

}

void VSTNode::onContainerParameterChanged ( ParameterBase* p)
{
    NodeBase::onContainerParameterChanged (p);

    if (p == identifierString)
    {
        if (identifierString->value != "")
        {
            jassert (!identifierString->checkValueIsTheSame (identifierString->value, identifierString->lastValue));
            PluginDescription* pd = VSTManager::getInstance()->knownPluginList.getTypeForIdentifierString (identifierString->value);

            if (pd)
            {
#ifdef VSTLOADING_THREADED
                NodeManager::getInstance()->addJob (new VSTLoaderJob (pd, this), true);
#else
                suspendProcessing (true);
                generatePluginFromDescription (pd);
                DBG ("VST generated");
                needParamReload = true;
                triggerAsyncUpdate();

#endif
            }

            else
            {
                DBG ("VST : cant find plugin for identifier : " + identifierString->value.toString());
            }

        }
        else {DBG ("VST : no identifierString provided");}
    }
    //    else if (p == midiPortNameParam)
    //    {
    //        setCurrentDevice (midiPortNameParam->stringValue());
    //
    //    }

    else if (p == enabledParam && innerPlugin)
    {
        // send NoteOff on disable
        if (!enabledParam->boolValue())
        {
            double ts = Time::getMillisecondCounterHiRes();

            for (int i = 1 ; i < 17 ; i++)
            {
                MidiMessage msg = MidiMessage::allNotesOff (i);
                msg.setTimeStamp (ts);
                messageCollector.handleIncomingMidiMessage (nullptr, msg);
            }

            //      incomingMidi.clear();
            //      AudioBuffer<float> buffer(jmax(innerPlugin->getTotalNumInputChannels(),innerPlugin->getTotalNumOutputChannels()),innerPlugin->getBlockSize());
            //      messageCollector.removeNextBlockOfMessages (incomingMidi, buffer.getNumSamples());
            //      innerPlugin->setPlayHead((AudioPlayHead*)TimeManager::getInstance());
            //      innerPlugin->processBlock(buffer, incomingMidi);
        }


    }
    else if (p == processWhenBypassed)
    {
        // pass to bool for fast access in callback;
        bProcessWhenBypassed = processWhenBypassed->boolValue();
    }

    // a VSTParameter is changed
    else
    {
        if (blockFeedback)return;

        const OwnedArray<juce::AudioProcessorParameter>& vstParams (innerPlugin->getParameters());
        
        for (int i = VSTParameters.size() - 1; i >= 0; --i)
        {
            if (VSTParameters.getUnchecked (i) == p)
            {
                if (auto* param = vstParams[i])
                    param->setValue (VSTParameters.getUnchecked (i)->value);
                break;
            }

        }
    }
};

ParameterBase * generateFromVST(const AudioProcessorParameter* param){
    String name = param->getName(maxVSTParamNameSize);
    String label = param->getLabel();

    if(param->isBoolean()){
        return new BoolParameter (name, label, param->getValue ());
    }
    else if(param->isDiscrete()){
        return new IntParameter (name,label, param->getValue (),0,param->getNumSteps());
    }
    else{
        return new FloatParameter (name,label, param->getValue ());
    }


}
void VSTNode::initParametersFromProcessor (AudioPluginInstance* p)
{

    // will check if not already here
    p->addListener (this);
    {
        ScopedLock lk (controllables.getLock());

        for (auto& c : VSTParameters)
        {
            removeControllable (c);
        }

        VSTParameters.clear();
        VSTParameters.ensureStorageAllocated (p->getParameters().size());

        String name,label;
        for (auto param : p->getParameters())
        {
            VSTParameters.add(ParameterContainer::addParameter(generateFromVST(param)));
        }

    }


    vstNodeListeners.call (&VSTNodeListener::newVSTSelected);
}

void VSTNode::updateParametersFromProcessor (AudioPluginInstance* p)
{
    jassert(VSTParameters.size()==innerPlugin->getParameters().size());
    if(VSTParameters.size()==innerPlugin->getParameters().size()){
        int i = 0;
        int baseVSTPIdx = VSTParameters.size()?controllables.indexOf(VSTParameters.getUnchecked(0)):-1;
        for(auto *p:VSTParameters){
            bool res = false;
            const auto * p2 = innerPlugin->getParameters().getUnchecked(i);
            if(p2->getName(maxVSTParamNameSize)!=p->niceName){
                res = true;
            }
            else if(p2->isBoolean() && !dynamic_cast<BoolParameter*>(p)){
                res  =true;
            }
            else if(p2->isDiscrete() && !p2->isBoolean() && !dynamic_cast<IntParameter*>(p)){
                res =  true;
            }
            if(res){
                ScopedLock lk (controllables.getLock());
                VSTParameters.set(i ,ParameterContainer::addParameter(generateFromVST(p2),baseVSTPIdx+i));
            }
            i++;
        }
    }
    vstNodeListeners.call (&VSTNodeListener::newVSTSelected);
}


/*
 String VSTNode::getPresetFilter()
 {
 return identifierString->stringValue();
 }
 */


void VSTNode::generatePluginFromDescription (PluginDescription* desc)
{

    //  closePluginWindow();
    innerPlugin = nullptr;
    String errorMessage;
    AudioDeviceManager::AudioDeviceSetup result;

    // set max channels to this
    desc->numInputChannels = jmin (desc->numInputChannels, getTotalNumInputChannels());
    desc->numOutputChannels = jmin (desc->numOutputChannels, getTotalNumOutputChannels());


    getAudioDeviceManager().getAudioDeviceSetup (result);

    if (AudioPluginInstance* instance = VSTManager::getInstance()->formatManager.createPluginInstance
        (*desc, result.sampleRate, result.bufferSize, errorMessage))
    {
        // try to align the precision of the processor and the graph


        // if it triggers an assert it's that vst is wrongly implemened (and there are a lot...)
        // ignoring the assert seems fair enough for now (juce_VSTPluginFormat.cpp l:794 while checking doubleprecision)

        instance->setProcessingPrecision (singlePrecision);
        instance->prepareToPlay (result.sampleRate, result.bufferSize);

        int numIn = instance->getTotalNumInputChannels();
        int numOut = instance->getTotalNumOutputChannels();
        //        NodeBase::setPlayConfigDetails(numIn, numOut, result.sampleRate, result.bufferSize);
        setPreferedNumAudioInput (numIn);
        setPreferedNumAudioOutput (numOut);

        innerPluginTotalNumInputChannels = instance->getTotalNumInputChannels();
        innerPluginTotalNumOutputChannels = instance->getTotalNumOutputChannels();
        innerPluginMaxCommonChannels = jmin (innerPluginTotalNumInputChannels, innerPluginTotalNumOutputChannels);
        DBG ("buffer sizes" + String (instance->getTotalNumInputChannels()) + ',' + String (instance->getTotalNumOutputChannels()));

        instance->setPlayHead (getPlayHead());

        innerPlugin = instance;
        messageCollector.reset (result.sampleRate);


    }

    else
    {
        innerPluginTotalNumInputChannels = 0;
        innerPluginTotalNumOutputChannels = 0;
        innerPlugin = nullptr;
        LOGE(errorMessage);
        jassertfalse;
    }
}

void VSTNode::audioProcessorChanged (juce::AudioProcessor* p )
{
    if (!innerPlugin || p != innerPlugin) return;

    if(p->getParameters().size()!=VSTParameters.size())
    {
        NLOG("VSTNode : " + innerPlugin->getName(), "rebuilding Parameters");
        initParametersFromProcessor (innerPlugin);
    }
    else
    {
        updateParametersFromProcessor(innerPlugin);
//        const OwnedArray<juce::AudioProcessorParameter>& vstParams (innerPlugin->getParameters());
//
//        for (int i = 0 ; i < VSTParameters.size() ; i++)
//        {
//            auto * param = vstParams.getUnchecked (i);
//            VSTParameters.getUnchecked (i)->setValue (param->getValue());
//            VSTParameters.getUnchecked (i)->setNiceName (param->getName (maxVSTParamNameSize));
//        }
    }

}
void VSTNode::numChannelsChanged (bool /*isInput*/) {}

inline void VSTNode::processBlockInternal (AudioBuffer<float>& buffer, MidiBuffer& )
{
    if (innerPlugin)
    {
        incomingMidi.clear();
        messageCollector.removeNextBlockOfMessages (incomingMidi, buffer.getNumSamples());
        innerPlugin->setPlayHead ((AudioPlayHead*)TimeManager::getInstance());
        innerPlugin->processBlock (buffer, incomingMidi);
    }
}


void VSTNode::audioProcessorParameterChanged (AudioProcessor* p,
                                              int parameterIndex,
                                              float newValue)
{
    if (p == innerPlugin)
    {
        jassert (parameterIndex < VSTParameters.size());
        blockFeedback = true;
        const OwnedArray<AudioProcessorParameter>& innerPL ( innerPlugin->getParameters());
        if (parameterIndex < VSTParameters.size() && parameterIndex<innerPL.size() &&
            VSTParameters.getUnchecked (parameterIndex)->niceName == innerPL.getUnchecked (parameterIndex)->getName(maxVSTParamNameSize))
        {
            VSTParameters.getUnchecked (parameterIndex)->setValue (newValue);
            blockFeedback = false;
        }
        else
        {
            NLOGE("VSTNode", "oldParam update");
        }
    }
    else
    {
        NLOGE("VSTNode", "oldplugin update");
    }
}

bool VSTNode::parameterHaveChanged(){
    if(VSTParameters.size()==innerPlugin->getParameters().size()){
        int i = 0;
        for(auto & p:VSTParameters){
            const auto & p2 = innerPlugin->getParameters().getUnchecked(i);
            if(p2->getName(maxVSTParamNameSize)!=p->niceName){
                return true;
            }
            else if(p2->isBoolean() && !dynamic_cast<BoolParameter*>(p)){
                return true;
            }
            else if(p2->isDiscrete() && !p2->isBoolean() && !dynamic_cast<IntParameter*>(p)){
                return true;
            }

            i++;
        }
        return false;
    }
    else{
        return true;
    }
}


void VSTNode::handleIncomingMidiMessage (MidiInput*,
                                         const MidiMessage& message)
{
    if (innerPlugin)
    {
        messageCollector.addMessageToQueue (message);
    }

    midiActivityTrigger->trigger();
};

void VSTNode::getStateInformation (MemoryBlock& destData)
{
    if (innerPlugin)
    {
        innerPlugin->getStateInformation (destData);
    };
}

void VSTNode::setStateInformation (const void* data, int sizeInBytes)
{

    MemoryBlock  nb(data,sizeInBytes);
    const ScopedLock lk(pluginStateMutex);
    if ( nb!=stateInfo)

    {
        stateInfo.replaceWith (data,sizeInBytes);
        triggerAsyncUpdate();

    };
};

void VSTNode::loadPresetInternal (PresetManager::Preset* preset)
{

    presetToLoad = preset;

    var v = presetToLoad->getPresetValue ("/rawData");
    jassert (v.isUndefined() || v.isString());
    MemoryBlock m;
    m.fromBase64Encoding (v.toString());
    setStateInformation (m.getData(), (int)m.getSize());

};


void VSTNode::savePresetInternal (PresetManager::Preset* preset)
{

    MemoryBlock m;
    getStateInformation (m);
    preset->addPresetValue ("/rawData", var (m.toBase64Encoding()));

};

void VSTNode::handleAsyncUpdate()
{
    bool inOutChanged = true;
    bool numParamChanged = true;

    if(innerPlugin && stateInfo.getSize() ){
        inOutChanged = false;
        const ScopedLock lk(pluginStateMutex);
        int oldNumInput = innerPlugin->getTotalNumInputChannels();
        int oldNumOutput = innerPlugin->getTotalNumOutputChannels();

        DBG ("loading state for vst " + getNiceName() + (parentContainer ? "in : " + parentContainer->getNiceName() : ""));
        innerPlugin->setStateInformation (stateInfo.getData() ,(int) stateInfo.getSize());

        inOutChanged |= oldNumInput!=innerPlugin->getTotalNumInputChannels();
        inOutChanged |= oldNumOutput!=innerPlugin->getTotalNumOutputChannels();
        numParamChanged = innerPlugin->getParameters().size() !=VSTParameters.size();
        stateInfo.reset();


    }

    if (inOutChanged && parentNodeContainer)  parentNodeContainer->updateAudioGraph();

    if (numParamChanged && innerPlugin)   initParametersFromProcessor (innerPlugin);
    else updateParametersFromProcessor(innerPlugin);
    
    needParamReload = false;
    suspendProcessing (false);
}
