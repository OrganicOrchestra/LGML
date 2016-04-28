/*
 ==============================================================================

 AudioMixerNodeUI.h
 Created: 24 Apr 2016 8:37:34pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef AUDIOMIXERNODEUI_H_INCLUDED
#define AUDIOMIXERNODEUI_H_INCLUDED


#include "AudioMixerNode.h"
#include "NodeBaseContentUI.h"


#include "FloatSliderUI.h"

class AudioMixerNodeUI : public NodeBaseContentUI,public NodeAudioProcessor::NodeAudioProcessorListener{
public:
    AudioMixerNodeUI(){

    }

    ~AudioMixerNodeUI(){
        nodeMixer->removeNodeAudioProcessorListener(this);
    }
    void init() override{
        nodeMixer = dynamic_cast<AudioMixerNode::AudioMixerAudioProcessor*>(node->audioProcessor);
        numAudioOutputChanged(nodeMixer->numberOfOutput->value);
        numAudioInputChanged(nodeMixer->numberOfInput->value);

        nodeMixer->addNodeAudioProcessorListener(this);

        nodeUI->setSize(250, 150);
    }

    void numAudioInputChanged(int )override;
    void numAudioOutputChanged(int )override;

    class OutputBusUI : public Component {
    public:

        OwnedArray<FloatSliderUI> inputVolumes;


        OutputBusUI(AudioMixerNode::AudioMixerAudioProcessor::OutputBus * o):owner(o){
            setNumInput(o->volumes.size());
        };
        ~OutputBusUI(){
        }
        void setNumInput(int numInput);
        void resized() override;
        int outputIdx;
        AudioMixerNode::AudioMixerAudioProcessor::OutputBus* owner;


    };


    void resized() override;
    OwnedArray<OutputBusUI> outputBusUIs;
    AudioMixerNode::AudioMixerAudioProcessor * nodeMixer;
    
};



#endif  // AUDIOMIXERNODEUI_H_INCLUDED
