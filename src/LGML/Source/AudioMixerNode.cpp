/*
 ==============================================================================

 AudioMixerNode.cpp
 Created: 3 Mar 2016 10:14:46am
 Author:  bkupe

 ==============================================================================
 */

#include "AudioMixerNode.h"
#include "NodeBaseUI.h"

#include "AudioMixerNodeUI.h"

NodeBaseUI * AudioMixerNode::createUI()
{

    NodeBaseUI * ui = new NodeBaseUI(this,new AudioMixerNodeUI);
    return ui;

}


AudioMixerNode::AudioMixerNode(NodeManager * nodeManager,uint32 nodeId) :NodeBase(nodeManager,nodeId, "AudioMixer", new AudioMixerAudioProcessor, nullptr) {

    addChildControllableContainer((AudioMixerAudioProcessor*) audioProcessor);
}




//==============================================================================
//    AudioMixerAudioProcessor


AudioMixerNode::AudioMixerAudioProcessor::AudioMixerAudioProcessor():NodeAudioProcessor(),ControllableContainer("AudioMixer"){

    numberOfInput = addIntParameter("numInput", "number of input", 0, 0, 32);
    numberOfOutput = addIntParameter("numOutput", "number of output", 0, 0, 32);
    numberOfOutput->setValue(2);
    numberOfInput->setValue(8);
    skipControllableNameInAddress = true;
};

void AudioMixerNode::AudioMixerAudioProcessor::onContainerParameterChanged(Parameter *p){
    if(p == numberOfInput){
        updateInput();
    }
    else if(p==numberOfOutput){
        updateOutput();
    }

}

void AudioMixerNode::AudioMixerAudioProcessor::updateInput(){

        const ScopedLock sl (getCallbackLock());
        suspendProcessing(true);
        for(auto & bus:outBuses){
            bus->setNumInput(numberOfInput->value);
        }


    setPreferedNumAudioInput(numberOfInput->value);
    suspendProcessing(false);

}

void AudioMixerNode::AudioMixerAudioProcessor::updateOutput(){
    {
        const ScopedLock sl (getCallbackLock());

        if(numberOfOutput->intValue() > outBuses.size())
		{
            for(int i = outBuses.size() ; i < numberOfOutput->intValue() ; i++){
                OutputBus * outB = new OutputBus(i,numberOfInput->value);
                outBuses.add(outB);
                addChildControllableContainer(outB);
            }
        }else if(numberOfOutput->intValue() < outBuses.size())
		{
            for(int i = numberOfOutput->value;i<outBuses.size() ; i++){
                OutputBus * outB = outBuses.getUnchecked(i);
                removeChildControllableContainer(outB);
            }
            outBuses.removeRange(numberOfOutput->value, outBuses.size()-numberOfOutput->intValue());
        }
    }

    setPreferedNumAudioOutput(numberOfOutput->value);


}

void AudioMixerNode::AudioMixerAudioProcessor::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer&) {

    int numInput = getTotalNumInputChannels();

    if(!(outBuses.size()<=buffer.getNumChannels())){DBG("mixer : dropping frame");return;}
    int numSamples =  buffer.getNumSamples();

    // doesnt do anything if it's already the right size
    cachedBuffer.setSize(outBuses.size(), numSamples);


    if(numInput>0){
        for(int i = outBuses.size() -1 ; i >=0 ; --i){
            cachedBuffer.copyFromWithRamp(i, 0, buffer.getReadPointer(0),numSamples,outBuses[i]->lastVolumes[0],outBuses[i]->volumes[0]->value);

            for(int j = numInput-1 ; j >0  ; --j){
                cachedBuffer.addFromWithRamp(i, 0, buffer.getReadPointer(j),numSamples, outBuses[i]->lastVolumes[j],outBuses[i]->volumes[j]->value);
            }


            for(int j = numInput-1 ; j>=0 ;--j){
                outBuses[i]->lastVolumes.set(j, outBuses[i]->volumes[j]->value);
            }
        }
    }

    for(int i = 0 ; i < cachedBuffer.getNumChannels() ; i++){
        buffer.copyFrom(i, 0, cachedBuffer.getReadPointer(i), buffer.getNumSamples());
    }

    // if buffer is bigger than cached (input>output) excedent it should never be used after this call so reference it
    //    buffer.setDataToReferTo(cachedBuffer.getArrayOfWritePointers(), cachedBuffer.getNumChannels(), cachedBuffer.getNumSamples());



}


//==============================================================================
// output bus

AudioMixerNode::AudioMixerAudioProcessor::OutputBus::OutputBus(int _outputIndex,int numInput):
outputIndex(_outputIndex),
ControllableContainer("outputBus : "+String(_outputIndex)){
    setNumInput(numInput);

}


void AudioMixerNode::AudioMixerAudioProcessor::OutputBus::setNumInput(int numInput){

    if(numInput>volumes.size()){
        for(int i = volumes.size();i<numInput ; i++){
            volumes.add(addFloatParameter("volume "+String(i), "mixer volume from input"+String(i), 1.0f));
        }
    }
    else if(numInput<volumes.size()){
        jassert(numInput>=0);

        for(int i = volumes.size()-1;i>=numInput;i--){
            removeControllable(volumes[i]);
            volumes.removeLast();
        }
    }


    lastVolumes.resize(numInput);
    for(auto &v:volumes){v->setValue( 1.0f);}
}
