/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

/*
 NodeBase is the base class for all Nodes
 it contains NodeAudioProcessor and/or NodeBase::NodeDataProcessor


 */
#ifndef NODEBASE_H_INCLUDED
#define NODEBASE_H_INCLUDED
#pragma once


#include "ConnectableNode.h"
#include "../Audio/AudioHelpers.h"




class NodeBase :
    public ConnectableNode,
    public ReferenceCountedObject,
    public juce::AudioProcessor

{

public:
    NodeBase (const String& name = "[NodeBase]", bool _hasMainAudioControl = true);
    void remove();


    virtual bool hasAudioInputs() override;
    virtual bool hasAudioOutputs() override;


    //  TODO:  this should not be implemented in Node to avoid overriding this method
    void onContainerParameterChanged ( ParameterBase* p) override;
    void onContainerParameterChangedAsync ( ParameterBase*, const var& /*value*/)override {};


    virtual void clear() override;
    /** can be oerriden to react to clear */
    virtual void clearInternal() {};

    String getPresetFilter() override;
    DynamicObject* getObject() override;
    void configureFromObject (DynamicObject* data) override;




    virtual const String getName() const override;


    ///////////
    //AUDIO PROCESSOR
    ///////////


    virtual bool setPreferedNumAudioInput (int num);
    virtual bool setPreferedNumAudioOutput (int num);
    // this will be called with audio locked so that one can safely update internal variables used by processBlockInternal
    void numChannelsChanged() override {numChannelsChanged (true); numChannelsChanged (false);};
    virtual void numChannelsChanged (bool /*isInput*/) {};


    virtual void prepareToPlay (double, int) override {};
    virtual void releaseResources() override {};

    //bool silenceInProducesSilenceOut() const override { return false; }

    virtual AudioProcessorEditor* createEditor() override { return nullptr; }
    virtual bool hasEditor() const override { return false; }



    // dumb overrides from JUCE AudioProcessor :  MIDI
    int getNumPrograms() override { return 0; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const String getProgramName (int) override { return "NoProgram"; }
    void changeProgramName (int, const String&) override {};
    double getTailLengthSeconds() const override { return 0; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }



    // save procedures from host
    virtual void getStateInformation (juce::MemoryBlock&) override {};
    virtual void setStateInformation (const void*, int) override {};

    virtual void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
    virtual void processBlockInternal (AudioBuffer<float>& /*buffer*/, MidiBuffer& /*midiMessage*/ ) {};
    virtual void processBlockBypassed (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;


    int maxCommonIOChannels = 0;

    float globalRMSValueIn ;
    float globalRMSValueOut ;

protected:
    // A node need to be removed with it's remove function
    virtual ~NodeBase();
    void setParentNodeContainer (NodeContainer* _parentNodeContainer)override;

private:

    AudioProcessorGraph::Node*   getAudioNode();
    friend class NodeConnection;
    virtual void addToAudioGraph (AudioProcessorGraph*);
    virtual void removeFromAudioGraph();
    AudioProcessorGraph::Node::Ptr audioNode;
    AudioProcessor* getAudioProcessor();

    WeakReference<NodeBase>::Master masterReference;
    friend class WeakReference<NodeBase>;

    FadeInOut dryWetFader, muteFader;

    double lastDryVolume;
    bool wasEnabled;
    AudioBuffer<float> crossFadeBuffer;

    //RMS
    const float alphaRMS = 0.05f;
    const int samplesBeforeRMSUpdate = 512;
    int curSamplesForRMSInUpdate = 0;
    int curSamplesForRMSOutUpdate = 0;


    Array<float> rmsValuesIn;
    Array<float> rmsValuesOut;

    SmoothedValue<floatParamType> logVolume;
    float lastVolume;
    friend class RMSTimer;

    class RMSTimer : public Timer
    {
    public:
        RMSTimer (NodeBase* n): owner (n)
        {
            startTimerHz (30);
        }
        void timerCallback()override
        {
            owner->ConnectableNode::rmsListeners.call (&ConnectableNode::RMSListener::RMSChanged, owner->globalRMSValueIn, owner->globalRMSValueOut);

            for (int i = 0; i < owner->getTotalNumInputChannels(); i++)
            {
                owner->ConnectableNode::rmsChannelListeners.call (&ConnectableNode::RMSChannelListener::channelRMSInChanged, owner, owner->rmsValuesIn[i], i);
            }

            for (int i = 0; i < owner->getTotalNumOutputChannels(); i++)
            {
                owner->ConnectableNode::rmsChannelListeners.call (&ConnectableNode::RMSChannelListener::channelRMSOutChanged, owner, owner->rmsValuesOut[i], i);
            }

        }
        NodeBase* owner;
    };

    RMSTimer rmsTimer;
    friend class Engine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeBase)
    friend struct ContainerDeletePolicy<NodeBase>;
};



namespace juce
{
template<>
struct ContainerDeletePolicy<NodeBase>
{
    static void destroy (NodeBase* object)
    {
        ignoreUnused (sizeof (NodeBase));

        object->remove();
    }

};
}



#endif  // NODEBASE_H_INCLUDED
