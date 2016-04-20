/*
  ==============================================================================

    AudioInNode.h
    Created: 7 Mar 2016 8:03:48pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef AUDIOINNODE_H_INCLUDED
#define AUDIOINNODE_H_INCLUDED



#include "NodeBase.h"

AudioDeviceManager & getAudioDeviceManager() ;

class AudioInNode : public NodeBase,public ChangeListener
{

public:


    class AudioInProcessor : public juce::AudioProcessorGraph::AudioGraphIOProcessor, public NodeBase::NodeAudioProcessor
    {
    public:
        AudioInProcessor():
        NodeBase::NodeAudioProcessor(),
        AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType::audioInputNode)
        {
        NodeBase::NodeAudioProcessor::busArrangement.inputBuses.clear();

        }

        ~AudioInProcessor(){


        }

        void processBlockInternal(AudioBuffer<float>& buffer,MidiBuffer& midiMessages)override {
            AudioProcessorGraph::AudioGraphIOProcessor::processBlock(buffer, midiMessages);

        }


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioInProcessor)
    };





    void changeListenerCallback (ChangeBroadcaster* source)override;
    void setIOFromAudioDevice();

    AudioInNode(NodeManager * nodeManager,uint32 nodeId)  :
    NodeBase(nodeManager,nodeId,"AudioInNode",new AudioInProcessor){
        getAudioDeviceManager().addChangeListener(this);
        setIOFromAudioDevice();
    };
    ~AudioInNode(){getAudioDeviceManager().removeChangeListener(this);};

    virtual NodeBaseUI * createUI() override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioInNode)
};







#endif  // AUDIOINNODE_H_INCLUDED
