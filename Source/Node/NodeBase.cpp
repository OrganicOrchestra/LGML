/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#include "NodeBase.h"
#include "Manager/NodeManager.h"
#include "../Time/TimeManager.h"

#include "../Audio/AudioHelpers.h"

NodeBase::NodeBase (const String& name, bool _hasMainAudioControl) :
    ConnectableNode (name, _hasMainAudioControl),
    audioNode (nullptr),
    dryWetFader (5000, 5000, false, 1),
    muteFader (1000, 1000, false, 1),
    lastDryVolume (0),
    globalRMSValueIn (0),
    globalRMSValueOut (0),
    wasEnabled (false),
    logVolume (float01ToGain (DB0_FOR_01), 0.5),
    rmsTimer (this)

{



    lastVolume = hasMainAudioControl ? outputVolume->floatValue() : 0;
    dryWetFader.setFadedIn();
    muteFader.startFadeIn();


    for (int i = 0; i < 2; i++) rmsValuesIn.add (0);

    for (int i = 0; i < 2; i++) rmsValuesIn.add (0);

    // when audioNode gets deleted, it will try to remove this instance already deleting by itself
    //  incReferenceCount();

}


NodeBase::~NodeBase()
{


    if (audioNode.get())
    {
        jassertfalse;

    }

    rmsTimer.stopTimer();
    NodeBase::masterReference.clear();
    clear();

    // avoid removing from ConnectableNodeDestructor
    parentNodeContainer = nullptr;



}

const String NodeBase::getName() const
{
    return nameParam->stringValue();
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
    return getTotalNumInputData() > 0;
}

bool NodeBase::hasDataOutputs()
{
    //to override
    return getTotalNumOutputData() > 0;
}


void NodeBase::onContainerParameterChanged (Parameter* p)
{
    if (!p)return;

    ConnectableNode::onContainerParameterChanged (p);

    if (p == outputVolume)
    {
        logVolume.set ( float01ToGain (outputVolume->floatValue()));
    }

    //ENABLE PARAM ACT AS A BYPASS

    if (p == enabledParam)
    {
        if (enabledParam->boolValue()) {dryWetFader.startFadeIn();}
        else {dryWetFader.startFadeOut();}
    }

}

void NodeBase::clear()
{
    clearInternal();

    //Data
    inputDatas.clear();
    outputDatas.clear();
    rmsTimer.stopTimer();

    //removeFromAudioGraph();
}




//Save / Load

String NodeBase::getPresetFilter()
{
    return getTypeName() + String ("_") + uid.toString();
}

DynamicObject* NodeBase::getObject()
{
    auto data = ConnectableNode::getObject();

    MemoryBlock m;

    // TODO we could implement that for all node objects to be able to save any kind of custom data
    getStateInformation (m);

    if (m.getSize())
    {
        var audioProcessorData (new DynamicObject());
        audioProcessorData.getDynamicObject()->setProperty ("state", m.toBase64Encoding());
        data->setProperty ("audioProcessor", audioProcessorData);
    }

    return data;
}

void NodeBase::configureFromObject (DynamicObject* data)
{
    ConnectableNode::configureFromObject (data);

    var audioProcessorData = data->getProperty ("audioProcessor");
    String audioProcessorStateData = audioProcessorData.getProperty ("state", var());

    MemoryBlock m;
    m.fromBase64Encoding (audioProcessorStateData);
    setStateInformation (m.getData(), (int)m.getSize());
}





/////////////////////////////////////// AUDIO


void NodeBase::setParentNodeContainer (NodeContainer* _parentNodeContainer)
{
    ConnectableNode::setParentNodeContainer (_parentNodeContainer);
    addToAudioGraph (_parentNodeContainer->getAudioGraph());
}

AudioProcessorGraph::Node* NodeBase::getAudioNode()
{
    jassert (audioNode->getProcessor() == getAudioProcessor());
    return audioNode.get();
}


void NodeBase::addToAudioGraph (AudioProcessorGraph* g)
{
    audioNode = g->addNode (getAudioProcessor());
    getAudioProcessor()->setRateAndBufferSizeDetails (g->getSampleRate(), g->getBlockSize());

}

void NodeBase::removeFromAudioGraph()
{
    if (parentNodeContainer)
    {
        if (auto pG = parentNodeContainer->getAudioGraph())
        {
            const ScopedLock lk (pG->getCallbackLock());
            pG->removeNode (audioNode);
        }

        parentNodeContainer->updateAudioGraph (false);
    }
}

AudioProcessor* NodeBase::getAudioProcessor()
{
    if (audioNode)
    {
        jassert (audioNode->getProcessor() == this);
        return audioNode->getProcessor();
    }

    return this;
};

void NodeBase::processBlockBypassed (AudioBuffer<float>& /*buffer*/, juce::MidiBuffer& /*midiMessages*/)
{
    // no op
}

void NodeBase::processBlock (AudioBuffer<float>& buffer,
                             MidiBuffer& midiMessages)
{

    // be sure to delete input if we are not enabled and a random buffer enters
    // juceAudioGraph seems to use the fact that we shouldn't process audio to pass others
    int numSample = buffer.getNumSamples();


    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();




    if (rmsListeners.size() || rmsChannelListeners.size())
    {
        curSamplesForRMSInUpdate += numSample;

        if (curSamplesForRMSInUpdate >= samplesBeforeRMSUpdate)
        {
            updateRMS (buffer, globalRMSValueIn, rmsValuesIn, totalNumInputChannels, rmsChannelListeners.size() == 0);
            curSamplesForRMSInUpdate = 0;
        }
    }



    muteFader.incrementFade (numSample);
    dryWetFader.incrementFade (numSample);
    logVolume.update();
    const double crossfadeValue = dryWetFader.getCurrentFade();
    const double muteFadeValue = muteFader.getCurrentFade();
    const double curLogVol = logVolume.get();

    // on disable
    if (wasEnabled && crossfadeValue == 0 )
    {


        wasEnabled = false;
    }

    // on Enable
    if (!wasEnabled && crossfadeValue > 0 )
    {

        wasEnabled = true;
    }

    if (!isSuspended())
    {

        double curVolume = curLogVol * crossfadeValue * muteFadeValue;
        double curDryVolume = curLogVol * (1.0 - crossfadeValue) * muteFadeValue;

        if (crossfadeValue != 1)
        {
            // copy only what we are expecting
            int maxCommonChannels = jmin (totalNumOutputChannels, totalNumInputChannels);
            crossFadeBuffer.setSize (maxCommonChannels, numSample);

            for (int i = 0 ; i < maxCommonChannels ; i++)
            {
                crossFadeBuffer.copyFrom (i, 0, buffer, i, 0, numSample);
            }
        }

        if (lastVolume == 0 && curVolume == 0)
        {
            processBlockBypassed (buffer, midiMessages);
        }
        else
        {
            processBlockInternal (buffer, midiMessages);
        }

        if (crossfadeValue != 1 || hasMainAudioControl)
        {
            buffer.applyGainRamp (0, numSample, lastVolume, (float)curVolume);

        }


        // crossfade if we have a dry mix i.e at least one input channel
        if (totalNumInputChannels > 0 && totalNumOutputChannels > 0)
        {
            if (crossfadeValue != 1 && crossFadeBuffer.getNumChannels() > 0)
            {
                for (int i = 0; i < totalNumOutputChannels; i++)
                {
                    buffer.addFromWithRamp (i, 0, crossFadeBuffer.getReadPointer (jmin (i, crossFadeBuffer.getNumChannels() - 1)), numSample, (float)lastDryVolume, (float)curDryVolume);
                }
            }
        }


        if (muteFadeValue == 0)
        {
            buffer.clear();
        }

        lastVolume = (float)curVolume;
        lastDryVolume = curDryVolume;

    }
    else
    {
        DBG ("suspended");
    }


    // be sure to delete out if we are not enabled and a random buffer enters
    // juceAudioGraph seems to use the fact that we shouldn't process audio to pass others
    //  for(int i = totalNumOutputChannels;i < buffer.getNumChannels() ; i++){
    //    buffer.clear(i,0,numSample);
    //  }

    if (rmsListeners.size() || rmsChannelListeners.size())
    {
        curSamplesForRMSOutUpdate += numSample;

        if (curSamplesForRMSOutUpdate >= samplesBeforeRMSUpdate)
        {
            updateRMS (buffer, globalRMSValueOut, rmsValuesOut, totalNumOutputChannels, rmsChannelListeners.size() == 0);
            curSamplesForRMSOutUpdate = 0;
        }
    }





};

bool NodeBase::setPreferedNumAudioInput (int num)
{

    int oldNumChannels = getTotalNumInputChannels();

    {

        if (parentNodeContainer != nullptr)
        {
            {
                const ScopedLock lk ( parentNodeContainer->innerGraph->getCallbackLock());
                setPlayConfigDetails (num, getTotalNumOutputChannels(),
                                      getSampleRate(),
                                      getBlockSize());


                parentNodeContainer->updateAudioGraph (false);

                if (oldNumChannels != getTotalNumInputChannels())
                {
                    // numChannelsChanged is called within the lock so that Nodes can update freely their memory used in processblock
                    numChannelsChanged (true);
                }
            }

        }
        else
        {
            // here is only if the Node sets a default prefered audio Input (in its constructor)
            setPlayConfigDetails (num, getTotalNumOutputChannels(),
                                  getSampleRate(),
                                  getBlockSize());

            if (oldNumChannels != getTotalNumInputChannels())
            {

                numChannelsChanged (true);
            }
        }
    }

    rmsValuesIn.clear();
    int totalNumInputChannels = getTotalNumInputChannels();

    for (int i = 0; i < totalNumInputChannels; i++) rmsValuesIn.add (0);


    if (totalNumInputChannels > oldNumChannels)
    {
        for (int i = oldNumChannels; i < totalNumInputChannels; i++)
        {
            nodeListeners.call (&ConnectableNodeListener::audioInputAdded, this, i);
        }
    }
    else
    {
        for (int i = oldNumChannels - 1; i >= totalNumInputChannels; i--)
        {
            nodeListeners.call (&ConnectableNodeListener::audioInputRemoved, this, i);
        }
    }


    nodeListeners.call (&ConnectableNodeListener::numAudioInputChanged, this, num);

    return true;
}


bool NodeBase::setPreferedNumAudioOutput (int num)
{

    int oldNumChannels = getTotalNumOutputChannels();
    {

        ScopedPointer<ScopedLock> lkp;

        if (parentNodeContainer != nullptr)
        {
            lkp = new ScopedLock (parentNodeContainer->getAudioGraph()->getCallbackLock());
        }

        setPlayConfigDetails (getTotalNumInputChannels(), num,
                              getSampleRate(),
                              getBlockSize());

        if (parentNodeContainer != nullptr)
        {
            parentNodeContainer->updateAudioGraph (false);
        }

        if (oldNumChannels != getTotalNumOutputChannels())
        {
            numChannelsChanged (false);
        }

    }

    rmsValuesOut.clear();

    int totalNumOutputChannels = getTotalNumOutputChannels();

    for (int i = 0; i < totalNumOutputChannels; i++) rmsValuesOut.add (0);


    if (totalNumOutputChannels > oldNumChannels)
    {
        for (int i = oldNumChannels; i < totalNumOutputChannels; i++)
        {
            nodeListeners.call (&ConnectableNodeListener::audioOutputAdded, this, i);
        }
    }
    else
    {
        for (int i = oldNumChannels - 1; i >= totalNumOutputChannels; i--)
        {
            nodeListeners.call (&ConnectableNodeListener::audioOutputRemoved, this, i);
        }
    }

    nodeListeners.call (&ConnectableNodeListener::numAudioOutputChanged, this, num);

    return true;
}






//////////////////////////////////DATA

Data* NodeBase::getInputData (int dataIndex)
{
    ScopedLock lk (numDataIOLock);

    if (inputDatas.size() <= dataIndex) return nullptr;

    return inputDatas[dataIndex];
}


Data* NodeBase::getOutputData (int dataIndex)
{
    ScopedLock lk (numDataIOLock);

    if (outputDatas.size() <= dataIndex) return nullptr;

    return outputDatas[dataIndex];
}


Data* NodeBase::addInputData (const String& name, Data::DataType dataType)
{
    ScopedLock lk (numDataIOLock);
    Data* d = new Data (this, name, dataType, Data::Input);
    inputDatas.add (d);

    d->addDataListener (this);

    nodeListeners.call (&ConnectableNodeListener::dataInputAdded, this, d);
    nodeListeners.call (&ConnectableNodeListener::numDataInputChanged, this, inputDatas.size());
    return d;
}

Data* NodeBase::addOutputData (const String& name, DataType dataType)
{
    ScopedLock lk (numDataIOLock);
    Data* d = new Data (this, name, dataType, Data::Output);
    outputDatas.add (d);

    d->addDataListener (this);

    nodeListeners.call (&ConnectableNodeListener::dataOutputAdded, this, d);
    nodeListeners.call (&ConnectableNodeListener::numDataOutputChanged, this, inputDatas.size());

    return d;
}

bool NodeBase::removeInputData (const String& name)
{
    ScopedLock lk (numDataIOLock);
    Data* d = getInputDataByName (name);

    if (d == nullptr) return false;

    d->removeDataListener (this);
    inputDatas.removeObject (d, false);
    nodeListeners.call (&ConnectableNodeListener::dataInputRemoved, this, d);
    nodeListeners.call (&ConnectableNodeListener::numDataInputChanged, this, inputDatas.size());
    delete d;
    return true;
}

bool NodeBase::removeOutputData (const String& name)
{
    ScopedLock lk (numDataIOLock);
    Data* d = getOutputDataByName (name);

    if (d == nullptr) return false;

    d->removeDataListener (this);
    outputDatas.removeObject (d, false);
    nodeListeners.call (&ConnectableNodeListener::dataOutputRemoved, this, d);
    nodeListeners.call (&ConnectableNodeListener::numDataOutputChanged, this, inputDatas.size());
    delete d;
    return true;
}

void NodeBase::removeAllInputDatas()
{
    while (inputDatas.size() > 0)
    {
        removeInputData (inputDatas[0]->name);
    }
}

void NodeBase::removeAllOutputDatas()
{
    while (outputDatas.size() > 0)
    {
        removeOutputData (outputDatas[0]->name);
    }
}

void NodeBase::updateOutputData (String& dataName, const float& value1, const float& value2, const float& value3)
{
    Data* d = getOutputDataByName (dataName);

    if (d != nullptr) d->update (value1, value2, value3);
}

int NodeBase::getTotalNumInputData()
{
    return inputDatas.size();
}

int NodeBase::getTotalNumOutputData()
{
    return outputDatas.size();
}

StringArray NodeBase::getInputDataInfos()
{
    StringArray dataInfos;

    for (auto& d : inputDatas) dataInfos.add (d->name + " (" + d->getTypeString() + ")");

    return dataInfos;
}

StringArray NodeBase::getOutputDataInfos()
{
    StringArray dataInfos;

    for (auto& d : outputDatas) dataInfos.add (d->name + " (" + d->getTypeString() + ")");

    return dataInfos;
}

Data::DataType NodeBase::getInputDataType (const String& dataName, const String& elementName)
{
    for (int i = inputDatas.size(); --i >= 0;)
    {
        Data* d = inputDatas.getUnchecked (i);

        if (d->name == dataName)
        {
            if (elementName.isEmpty())
            {
                return d->type;
            }
            else
            {
                DataElement* e = d->getElement (elementName);

                if (e == nullptr) return DataType::Unknown;

                return e->type;
            }
        }
    }

    return DataType::Unknown;
}

Data::DataType NodeBase::getOutputDataType (const String& dataName, const String& elementName)
{
    for (int i = outputDatas.size(); --i >= 0;)
    {
        Data* d = outputDatas.getUnchecked (i);

        if (d->name == dataName)
        {
            if (elementName.isEmpty())
            {
                return d->type;
            }
            else
            {
                DataElement* e = d->getElement (elementName);

                if (e == nullptr) return DataType::Unknown;

                return e->type;
            }
        }
    }

    return DataType::Unknown;

}

Data* NodeBase::getOutputDataByName (const String& dataName)
{
    for (auto& d : outputDatas)
    {
        if (d->name == dataName) return d;
    }

    return nullptr;
}

Data* NodeBase::getInputDataByName (const String& dataName)
{
    for (auto& d : inputDatas)
    {
        if (d->name == dataName) return d;
    }

    return nullptr;
}

void NodeBase::dataChanged (Data* d)
{
    //DBG("Data changed, ioType " << (d->ioType == Data::Input ? "input" : "output"));
    if (d->ioType == Data::Input)
    {
        if (enabledParam->boolValue())
        {
            processInputDataChanged (d);
        }

        nodeListeners.call (&ConnectableNodeListener::nodeInputDataChanged, this, d);
    }
    else
    {
        if (enabledParam->boolValue())
        {
            processOutputDataUpdated (d);
        }

        nodeListeners.call (&ConnectableNodeListener::nodeOutputDataUpdated, this, d);
    }
}

void NodeBase::remove()
{
    if(parentNodeContainer)
        parentNodeContainer->removeNode(this,false);
    
    if (audioNode.get())
    {
        removeFromAudioGraph();
        jassert (audioNode.get()->getReferenceCount() == 1);
        // audioNode is owning the pointer so triggers it's deletion instead
        audioNode = nullptr;
    }
    else
    {
        jassertfalse;
        delete this;
    }
}




