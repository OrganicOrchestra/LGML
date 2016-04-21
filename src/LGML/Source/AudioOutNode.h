/*
  ==============================================================================

    AudioOutNode.h
    Created: 7 Mar 2016 8:04:38pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef AUDIOOUTNODE_H_INCLUDED
#define AUDIOOUTNODE_H_INCLUDED



#include "NodeBase.h"


AudioDeviceManager & getAudioDeviceManager() ;


class AudioOutNode : public NodeBase,public ChangeListener
{
public:
    class AudioOutProcessor : public juce::AudioProcessorGraph::AudioGraphIOProcessor, public NodeBase::NodeAudioProcessor
    {
    public:
        AudioOutProcessor():
        NodeBase::NodeAudioProcessor(),
        AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType::audioOutputNode){
            NodeBase::NodeAudioProcessor::busArrangement.outputBuses.clear();
        }
        void processBlockInternal(AudioBuffer<float>& buffer,
                          MidiBuffer& midiMessages) {
            AudioProcessorGraph::AudioGraphIOProcessor::processBlock(buffer, midiMessages);
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioOutProcessor)
    };



    void changeListenerCallback (ChangeBroadcaster* source)override;
    void setIOFromAudioDevice();
    AudioOutNode(NodeManager * nodeManager,uint32 nodeId)  : NodeBase(nodeManager,nodeId,"AudioOutNode",new AudioOutProcessor){
        getAudioDeviceManager().addChangeListener(this);
        setIOFromAudioDevice();
    };
    ~AudioOutNode(){getAudioDeviceManager().removeChangeListener(this);};

    virtual NodeBaseUI * createUI() override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioOutNode)
};




#endif  // AUDIOOUTNODE_H_INCLUDED
