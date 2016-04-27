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

class AudioMixerNodeUI : public NodeBaseContentUI,public ControllableContainer::Listener{
public:
    AudioMixerNodeUI(){

    }

    ~AudioMixerNodeUI(){
        nodeMixer->removeControllableContainerListener(this);
    }
    void init() override{
        nodeMixer = dynamic_cast<AudioMixerNode::AudioMixerAudioProcessor*>(node->audioProcessor);
        for(auto & c:nodeMixer->outBuses){
            controllableContainerAdded(c);
        }

        nodeMixer->addControllableContainerListener(this);

        nodeUI->setSize(250, 150);
    }

    void controllableAdded(Controllable * c) override{};
    void controllableRemoved(Controllable * c) override{};
    void controllableContainerAdded(ControllableContainer * cc)override ;
    void controllableContainerRemoved(ControllableContainer * cc) override;
    void controllableFeedbackUpdate(Controllable *c) override{};

    class OutputBusUI : public Component ,ControllableContainer::Listener{
    public:

        OwnedArray<FloatSliderUI> inputVolumes;


        OutputBusUI(AudioMixerNode::AudioMixerAudioProcessor::OutputBus * o):owner(o){
            owner->addControllableContainerListener(this);
            setNumInput(o->volumes.size());
        };
        ~OutputBusUI(){
            owner->removeControllableContainerListener(this);
        }
        void setNumInput(int numInput);
        void resized() override;
        int outputIdx;
        AudioMixerNode::AudioMixerAudioProcessor::OutputBus* owner;

        void controllableAdded(Controllable * c) override;
        void controllableRemoved(Controllable * c) override;
        void controllableContainerAdded(ControllableContainer * cc)override {};
        void controllableContainerRemoved(ControllableContainer * cc) override{};
        void controllableFeedbackUpdate(Controllable *c) override{};

    };


    void resized() override;
    OwnedArray<OutputBusUI> outputBusUIs;
    AudioMixerNode::AudioMixerAudioProcessor * nodeMixer;
    
};



#endif  // AUDIOMIXERNODEUI_H_INCLUDED
