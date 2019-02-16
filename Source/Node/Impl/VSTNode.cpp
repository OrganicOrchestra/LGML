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
#include "../../Controllable/Parameter/ParameterFactory.h"
#include "../../Preset/Preset.h" // to ensure that changed param can be a VST one

constexpr int maxVSTParamNameSize = 100;

extern AudioDeviceManager& getAudioDeviceManager();

#define USE_JUCE_B64 1
String toBase64(MemoryBlock &m){
#if USE_JUCE_B64
    return m.toBase64Encoding();
#else
    return Base64::toBase64(m.getData(), m.getSize());
#endif
}

MemoryBlock fromBase64(const String & s){
#if USE_JUCE_B64
    MemoryBlock m;
    m.fromBase64Encoding(s);
    return m;
#else
    MemoryOutputStream os(s.length()*sizeof(s[0]));
    Base64::convertFromBase64(os, s);
    return os.getMemoryBlock();
#endif

}

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
    identifierString->setInternalOnlyFlags(false,true);

    addChildControllableContainer (&pluginWindowParameter);

    midiActivityTrigger =  addNewParameter<Trigger> ("Midi Activity", "Midi Activity indicator");
    midiActivityTrigger->isControllableExposed = false;
    //    midiPortNameParam = addNewParameter<StringParameter> ("midiPortName", "MIDI Port Name", "");
    //    midiPortNameParam->isHidenInEditor = true;

    processWhenBypassed = addNewParameter<BoolParameter> ("processWhenBypassed", "some effects (Reverbs ...) need to process constantly even when bypassed", false);
    bProcessWhenBypassed = processWhenBypassed->boolValue();

    stateInformation = addNewParameter<StringParameter>("stateInformation", "state of current plugin", "");
    stateInformation->setInternalOnlyFlags(true,true);
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
                if(stateInfoPluginID==pd->createIdentifierString())
                    setVSTState();
                else{
                    initParametersFromProcessor(innerPlugin);
                    getVSTState();
                }
                suspendProcessing (false);
                parentNodeContainer->updateAudioGraph();


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
    else if(p==stateInformation){
        setVSTState();
    }
    // a VSTParameter is changed
    else if(p!=nameParam && innerPlugin)
    {

        if (blockFeedback)return;

        const OwnedArray<juce::AudioProcessorParameter>& vstParams (innerPlugin->getParameters());
        if(VSTParameters.size()!=vstParams.size()){
            jassertfalse;
            return;
        }
        bool found = false;
        for (int i = VSTParameters.size() - 1; i >= 0; --i)
        {
            if (VSTParameters.getUnchecked (i) == p)
            {
                    vstParams.getUnchecked(i)->setValue (p->floatValue());
                found = true;
                //getVSTState();
                break;
            }

        }
        jassert(found);
    }
};



void VSTNode::setVSTState(){
    if(innerPlugin ){

        stateInfoPluginID = innerPlugin->getPluginDescription().createIdentifierString();

        bool inOutChanged = true;
        bool numParamChanged = true;

        if(stateInformation->stringValue().length()){
            if(stateInfoPluginID==innerPlugin->getPluginDescription().createIdentifierString()){
                inOutChanged = false;
                const ScopedLock lk(pluginStateMutex);
                int oldNumInput = innerPlugin->getTotalNumInputChannels();
                int oldNumOutput = innerPlugin->getTotalNumOutputChannels();

                DBG ("loading state for vst " + getNiceName() + (parentContainer ? "in : " + parentContainer->getNiceName() : ""));
                MemoryBlock stateInfo (fromBase64(stateInformation->stringValue()));
#if JUCE_DEBUG
                MemoryBlock currentInfo;
                innerPlugin->getStateInformation(currentInfo);
                jassert(currentInfo!=stateInfo);
#endif

                innerPlugin->setStateInformation (stateInfo.getData() ,(int) stateInfo.getSize());

                inOutChanged |= oldNumInput!=innerPlugin->getTotalNumInputChannels();
                inOutChanged |= oldNumOutput!=innerPlugin->getTotalNumOutputChannels();
                int curPSize = innerPlugin->getParameters().size();
                numParamChanged =  curPSize!=VSTParameters.size();

            }
            else{
                LOGE("trying to load old vst state");
                jassertfalse;
            }


        }


        if (inOutChanged && parentNodeContainer)  parentNodeContainer->updateAudioGraph();
        
        if (numParamChanged)   initParametersFromProcessor (innerPlugin);
        else updateParametersFromProcessor(innerPlugin);
        
        
        


    }
}

void VSTNode::getVSTState(){
    if(innerPlugin ){
        juce::MemoryBlock stateInfo;
        innerPlugin->getStateInformation(stateInfo);
        stateInfoPluginID = innerPlugin->getPluginDescription().createIdentifierString();
#if JUCE_DEBUG
        MemoryBlock currentInfo = fromBase64(stateInformation->stringValue());
        if(stateInfo!=currentInfo){
            int a = 0; a++;
        }
        else{
            int a = 0; a++;
//            jassertfalse;
        }
#endif

        stateInformation->setValueFrom(this,toBase64(stateInfo),true );
//        auto & vstP = innerPlugin->getParameters();
//        int i = 0;
//        for(auto &p : VSTParameters){
//            auto v = vstP.getUnchecked(i);
//            if(v->getValue()!=p->floatValue()){
//                v->setValue(p->floatValue());
//            }
//            i++;
//        }


    }
}

ParameterBase * VSTNode::generateFromVST(const AudioProcessorParameter* param){
    String name = param->getName(maxVSTParamNameSize);
    String label = param->getLabel();
    if(label.isEmpty())
        label =  String("parameter : ")+name;
    String uniqueName = getUniqueNameInContainer(name);
    ParameterBase * p;
    if(param->isBoolean()){

        p = new BoolParameter (uniqueName, label, (bool)param->getValue ());
    }
    else if(param->isDiscrete()){
        p = new IntParameter (uniqueName,label, param->getValue (),0,param->getNumSteps());
    }
    else{
        p = new FloatParameter (uniqueName,label, param->getValue ());
    }
    if(name!=p->niceName){
        DBG("renaming p : " + name + " >> " + p->niceName);
    }

    p->isSavableAsObject = true;

    return p;

}

ParameterBase * VSTNode::addParameterFromVar(const String & name,const var & data){
    auto p = ParameterContainer::addParameterFromVar(name, data);
    VSTParameters.add(p);
    return p;
}
void VSTNode::initParametersFromProcessor (AudioPluginInstance* pi)
{
    if(!pi){
        jassertfalse;
        return;
    }
    // will check if not already here
    pi->addListener (this);
    {
        ScopedLock lk (controllables.getLock());

        for (auto& c : VSTParameters)
        {
            removeControllable (c);
        }

        VSTParameters.clear();
        if(pi){
        VSTParameters.ensureStorageAllocated (pi->getParameters().size());

//        String name,label;
        for (const auto & param : pi->getParameters())
        {
            VSTParameters.add(ParameterContainer::addParameter(generateFromVST(param)));

        }
        }

    }


    vstNodeListeners.call (&VSTNodeListener::newVSTSelected);
}

void VSTNode::updateParametersFromProcessor (AudioPluginInstance* pi)
{
    if(!innerPlugin){
        jassert(VSTParameters.size()==0);
        return;
    }
    jassert(VSTParameters.size()==innerPlugin->getParameters().size());
    bool hadOneChange = false;

    ScopedLock lk (controllables.getLock());
    if(VSTParameters.size()==innerPlugin->getParameters().size()){
        int i = 0;
        int baseVSTPIdx = VSTParameters.size()?controllables.indexOf(VSTParameters.getUnchecked(0)):-1;
        for(auto *p:VSTParameters){
            bool res = false;
            bool isCast = false;
            const auto * p2 = innerPlugin->getParameters().getUnchecked(i);
            const auto vstPname = p2->getName(maxVSTParamNameSize);
            if(vstPname!=p->niceName && !p->niceName.startsWith(vstPname+" ")){
                res = true;
            }
            else if(p2->isBoolean() && !dynamic_cast<BoolParameter*>(p)){
                isCast = true;
            }
            else if(p2->isDiscrete() && !p2->isBoolean() && !dynamic_cast<IntParameter*>(p)){
                isCast = true;
            }
            if(res || isCast){
                float oldValue = p->floatValue();
                removeControllable(p);
                auto newP =generateFromVST(p2);
                VSTParameters.set(i , ParameterContainer::addParameter(newP,baseVSTPIdx+i));
                if(isCast)
                    newP->setValue(oldValue);

                hadOneChange = true;
            }

            i++;
        }
    }
//    if(hadOneChange)loadVSTState();
    vstNodeListeners.call (&VSTNodeListener::newVSTSelected);
}

//
//void VSTNode::updateParametersToProcessor(AudioPluginInstance* pi){
//    jassert(VSTParameters.size()==innerPlugin->getParameters().size());
//    ScopedLock lk (controllables.getLock());
//    if(VSTParameters.size()==innerPlugin->getParameters().size()){
//        int i = 0;
//        for(auto *p:VSTParameters){

//            auto * p2 = innerPlugin->getParameters().getUnchecked(i);
//            const auto p2Name = p2->getName(maxVSTParamNameSize);
//            if(p2Name==p->niceName  || p->niceName.startsWith(p2Name+" ")){
//                p2->setValue(p->floatValue());
//            }
//            else{
//                jassertfalse;
//            }
//            i++;
//        }
//    }
//
//    }




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
        jassertfalse;
        initParametersFromProcessor (innerPlugin);
        getVSTState();

    }
    else
    {
        if(parameterHaveChanged()){
        getVSTState();
//        updateParametersToProcessor(innerPlugin);
        updateParametersFromProcessor(innerPlugin);
        }

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
        if (parameterIndex < VSTParameters.size() && parameterIndex<innerPL.size()){

            auto * localP = VSTParameters.getUnchecked (parameterIndex);
            if(localP->niceName.startsWith(innerPL.getUnchecked (parameterIndex)->getName(maxVSTParamNameSize)))
            {
                localP->setValue (newValue);
                blockFeedback = false;
//                loadVSTState();
            }
            else
            {
                NLOGE("VSTNode", "oldParam update");
                jassertfalse;
            }
        }
        else{
            NLOGE("VSTNode","wrong param num update");
            jassertfalse;
        }
    }
    else
    {
        NLOGE("VSTNode", "oldplugin update");
        jassertfalse;
    }

}

bool VSTNode::parameterHaveChanged(){
    if(VSTParameters.size()==innerPlugin->getParameters().size()){
        int i = 0;
        for(auto & p:VSTParameters){
            const auto & p2 = innerPlugin->getParameters().getUnchecked(i);
            auto p2Name  = p2->getName(maxVSTParamNameSize);
            if(p2Name!=p->niceName && !p->niceName.startsWith(p2Name+" ")){
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


void VSTNode::handleAsyncUpdate()
{
}

String VSTNode::getSubTypeName() {
    if(innerPlugin){
        return innerPlugin->getPluginDescription().createIdentifierString();
    }
    return "";
}


