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

AudioMixerNode::AudioMixerNode(uint32 nodeId) :NodeBase(nodeId, "AudioMixerNode")
{
	numberOfInput = addIntParameter("numInput", "number of input", 8, 1, 32);
	numberOfOutput = addIntParameter("numOutput", "number of output", 2, 1, 16);

	updateInput();
	updateOutput();

}



void AudioMixerNode::onContainerParameterChanged(Parameter *p){
    if(p == numberOfInput){
        updateInput();
    }
    else if(p==numberOfOutput){
        updateOutput();
    }

}

void AudioMixerNode::updateInput(){
    {
        const ScopedLock sl (getCallbackLock());
        suspendProcessing(true);
        for(auto & bus:outBuses){
            bus->setNumInput(numberOfInput->intValue());
        }

    }
    setPreferedNumAudioInput(numberOfInput->intValue());
    suspendProcessing(false);

}

void AudioMixerNode::updateOutput(){
    {
        const ScopedLock sl (getCallbackLock());
        suspendProcessing(true);

        if(numberOfOutput->intValue() > outBuses.size())
        {
            for(int i = outBuses.size() ; i < numberOfOutput->intValue() ; i++){
                OutputBus * outB = new OutputBus(i,numberOfInput->intValue());
                outBuses.add(outB);
                addChildControllableContainer(outB);
            }
        }else if(numberOfOutput->intValue() < outBuses.size())
        {
            for(int i = numberOfOutput->intValue();i<outBuses.size() ; i++){
                OutputBus * outB = outBuses.getUnchecked(i);
                removeChildControllableContainer(outB);
            }
            outBuses.removeRange(numberOfOutput->intValue(), outBuses.size()-numberOfOutput->intValue());
        }
    }

    setPreferedNumAudioOutput(numberOfOutput->intValue());
    suspendProcessing(false);

}

void AudioMixerNode::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer&) {

    int numInput = getTotalNumInputChannels();
    int numOutput = getTotalNumOutputChannels();

    if(!(outBuses.size()<=buffer.getNumChannels()))
    {
        DBG("mixer : dropping frame");return;
    }

    int numSamples =  buffer.getNumSamples();

    // doesnt do anything if it's already the right size
    cachedBuffer.setSize(outBuses.size(), numSamples);


    if(numInput>0 && numOutput > 0){

        for(int i = outBuses.size() -1 ; i >=0 ; --i){
            cachedBuffer.copyFromWithRamp(i, 0, buffer.getReadPointer(0),numSamples,outBuses[i]->lastVolumes[0],outBuses[i]->volumes[0]->floatValue());

            for(int j = numInput-1 ; j >0  ; --j){
                cachedBuffer.addFromWithRamp(i, 0, buffer.getReadPointer(j),numSamples, outBuses[i]->lastVolumes[j],outBuses[i]->volumes[j]->floatValue());
            }


            for(int j = numInput-1 ; j>=0 ;--j){
                outBuses[i]->lastVolumes.set(j, outBuses[i]->volumes[j]->floatValue());
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

AudioMixerNode::OutputBus::OutputBus(int _outputIndex,int numInput):
outputIndex(_outputIndex),
ControllableContainer("outputBus : "+String(_outputIndex)){
    setNumInput(numInput);

}


void AudioMixerNode::OutputBus::setNumInput(int numInput){

    if(numInput>volumes.size()){
        for(int i = volumes.size();i<numInput ; i++){
            volumes.add(addFloatParameter("In "+String(i)+ " > Out "+String(outputIndex), "mixer volume from input"+String(i), 1.0f));
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




NodeBaseUI * AudioMixerNode::createUI()
{

	NodeBaseUI * ui = new NodeBaseUI(this, new AudioMixerNodeUI);
	ui->recursiveInspectionLevel = 2;
	return ui;


}