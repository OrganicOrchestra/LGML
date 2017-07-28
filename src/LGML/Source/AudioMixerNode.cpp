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
#include "AudioHelpers.h"
#include "NodeContainer.h"

AudioMixerNode::AudioMixerNode() :
NodeBase("AudioMixerNode",NodeType::AudioMixerType)
{
    numberOfInput = addNewParameter<IntParameter>("numInput", "number of input", 2, 1, 32);
    numberOfOutput = addNewParameter<IntParameter>("numOutput", "number of output", 2, 1, 16);
    oneToOne = addNewParameter<BoolParameter>("OneToOne", "is this mixer only concerned about one to one volumes (diagonal)", false);

	
	setPreferedNumAudioInput(numberOfInput->intValue());
	setPreferedNumAudioOutput(numberOfOutput->intValue());
	numChannelsChanged(true);
  numChannelsChanged(false);


}

void AudioMixerNode::setParentNodeContainer(NodeContainer * c){
  NodeBase::setParentNodeContainer(c);
  updateInput();
  updateOutput();
  numChannelsChanged(true);
  numChannelsChanged(false);

}

void AudioMixerNode::onContainerParameterChanged(Parameter *p){
    NodeBase::onContainerParameterChanged(p);
    if(p == numberOfInput){
        updateInput();
    }
    else if(p==numberOfOutput){
        updateOutput();
    }

}

void AudioMixerNode::updateInput(){
    
    setPreferedNumAudioInput(numberOfInput->intValue());
//    suspendProcessing(false);

}

void AudioMixerNode::numChannelsChanged(bool /*isInput*/){


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

  for(auto & bus:outBuses){
    bus->setNumInput(numberOfInput->intValue());
  }
}
void AudioMixerNode::updateOutput(){
//    {
//        const ScopedLock lk (parentNodeContainer->getCallbackLock());
//        suspendProcessing(true);
//
//        if(numberOfOutput->intValue() > outBuses.size())
//        {
//            for(int i = outBuses.size() ; i < numberOfOutput->intValue() ; i++){
//                OutputBus * outB = new OutputBus(i,numberOfInput->intValue());
//                outBuses.add(outB);
//                addChildControllableContainer(outB);
//            }
//        }else if(numberOfOutput->intValue() < outBuses.size())
//        {
//            for(int i = numberOfOutput->intValue();i<outBuses.size() ; i++){
//                OutputBus * outB = outBuses.getUnchecked(i);
//                removeChildControllableContainer(outB);
//            }
//            outBuses.removeRange(numberOfOutput->intValue(), outBuses.size()-numberOfOutput->intValue());
//        }
//    }

    setPreferedNumAudioOutput(numberOfOutput->intValue());
//    suspendProcessing(false);

}


void AudioMixerNode::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer&) {


  int numBufferChannels = buffer.getNumChannels();

    if(!(outBuses.size()<=numBufferChannels))
    {
        DBG("mixer : dropping frame");return;
    }

    int numSamples =  buffer.getNumSamples();

    // doesnt do anything if it's already the right size
    cachedBuffer.setSize(outBuses.size(), numSamples);
  int totalNumInputChannels= getTotalNumInputChannels();

    if(getTotalNumOutputChannels()>0 && totalNumInputChannels > 0){

        if(oneToOne->boolValue()){
            for(int i = outBuses.size() -1 ; i >=0 ; --i){
              auto outI = outBuses.getUnchecked(i);
                if(i<outI->volumes.size()){
                    cachedBuffer.copyFromWithRamp(i, 0, buffer.getReadPointer(0),numSamples,
                                                  outI->lastVolumes[i],outI->logVolumes[i]);
                }
                else{
                    cachedBuffer.clear(i, 0, numSamples);
                }
            }
        }
        else{


            for(int i = outBuses.size() -1 ; i >=0 ; --i){
              auto outI = outBuses.getUnchecked(i);
                cachedBuffer.copyFromWithRamp(i, 0, buffer.getReadPointer(0),numSamples,
                                              outI->lastVolumes[0],outI->logVolumes[0]);

                for(int j = totalNumInputChannels-1 ; j >0  ; --j){
                    cachedBuffer.addFromWithRamp(i, 0, buffer.getReadPointer(j),numSamples,
                                                 outI->lastVolumes[j],outI->logVolumes[j]);
                }
            }
        }


        for(int i = outBuses.size() -1 ; i >=0 ; --i){
          auto outI = outBuses.getUnchecked(i);
            for(int j = totalNumInputChannels-1 ; j>=0 ;--j){
                outI->lastVolumes.set(j, outI->logVolumes[j]);
            }

        }
    }

    for(int i = 0 ; i < cachedBuffer.getNumChannels() ; i++){
        buffer.copyFrom(i, 0, cachedBuffer.getReadPointer(i), buffer.getNumSamples());
    }

}


//==============================================================================
// output bus

AudioMixerNode::OutputBus::OutputBus(int _outputIndex,int numInput):
outputIndex(_outputIndex),
ControllableContainer("outputBus : "+String(_outputIndex)){
    setNumInput(numInput);
    setCustomShortName("Out_"+String(_outputIndex+1));

}


void AudioMixerNode::OutputBus::setNumInput(int numInput){
  ScopedLock lk(volumes.getLock());
    if(numInput>volumes.size()){
        for(int i = volumes.size();i<numInput ; i++){
            FloatParameter * p = addNewParameter<FloatParameter>("In "+String(i+1)+ " > Out "+String(outputIndex+1), "mixer volume from input"+String(i+1), i == outputIndex?DB0_FOR_01:0);
            p->setCustomShortName("In_"+String(i+1));
            p->defaultValue = DB0_FOR_01;
            volumes.add(p);
            logVolumes.add(float01ToGain(p->floatValue()));
        }
    }
    else if(numInput<volumes.size()){
        jassert(numInput>=0);

        for(int i = volumes.size()-1;i>=numInput;i--){
            removeControllable(volumes[i]);
            volumes.removeLast();
            logVolumes.removeLast();
        }
    }


    lastVolumes.resize(numInput);
}

void AudioMixerNode::OutputBus::onContainerParameterChanged(Parameter *p){
  ScopedLock lk(volumes.getLock());
    if(volumes.contains((FloatParameter*)p)){
        logVolumes.resize(volumes.size());
        int i = 0;
        for(auto & v:logVolumes){
            v = float01ToGain(volumes.getUnchecked(i)->floatValue());
            i++;
        }
    }
}



ConnectableNodeUI * AudioMixerNode::createUI()
{

    NodeBaseUI * ui = new NodeBaseUI(this, new AudioMixerNodeUI);
    ui->recursiveInspectionLevel = 2;
    return ui;


}
