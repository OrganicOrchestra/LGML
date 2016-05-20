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
#include "NodeBaseUI.h"

#include "FloatSliderUI.h"

class AudioMixerNodeUI : public NodeBaseContentUI,
	public NodeBase::NodeAudioProcessorListener
{
public:
    AudioMixerNodeUI(){

    }

	~AudioMixerNodeUI();
	void init() override;

    void numAudioInputChanged(NodeBase *,int )override;
    void numAudioOutputChanged(NodeBase*, int )override;

    class OutputBusUI : public Component {
    public:

        OwnedArray<FloatSliderUI> inputVolumes;


        OutputBusUI(AudioMixerNode::OutputBus * o):owner(o){
            setNumInput(o->volumes.size());
        };

        ~OutputBusUI(){

        }
        void setNumInput(int numInput);
        void resized() override;
        int outputIdx;
        AudioMixerNode::OutputBus* owner;


    };


    void resized() override;
    OwnedArray<OutputBusUI> outputBusUIs;
    AudioMixerNode * mixerNode;

};



#endif  // AUDIOMIXERNODEUI_H_INCLUDED
