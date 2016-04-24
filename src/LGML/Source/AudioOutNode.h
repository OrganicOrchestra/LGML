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


class AudioOutNode : public NodeBase
{
public:
    class AudioOutProcessor : public juce::AudioProcessorGraph::AudioGraphIOProcessor, public NodeAudioProcessor,public ChangeListener
    {
    public:
        AudioOutProcessor():
        NodeAudioProcessor(),
        AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType::audioOutputNode){
            NodeAudioProcessor::busArrangement.outputBuses.clear();
        }

        void changeListenerCallback (ChangeBroadcaster* source)override;
        void updateIO();
        void processBlockInternal(AudioBuffer<float>& buffer,
                          MidiBuffer& midiMessages) override{
            AudioProcessorGraph::AudioGraphIOProcessor::processBlock(buffer, midiMessages);
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioOutProcessor)
    };




    AudioOutNode(NodeManager * nodeManager,uint32 nodeId)  : NodeBase(nodeManager,nodeId,"AudioOutNode",new AudioOutProcessor){
        AudioOutProcessor* ap=(AudioOutProcessor*)audioProcessor;
        getAudioDeviceManager().addChangeListener(ap);
        ap->updateIO();
    };
    ~AudioOutNode(){getAudioDeviceManager().removeChangeListener((AudioOutProcessor*)audioProcessor);};

    virtual NodeBaseUI * createUI() override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioOutNode)
};




#endif  // AUDIOOUTNODE_H_INCLUDED
