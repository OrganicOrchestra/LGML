/*
 ==============================================================================

 AudioDeviceInNode.cpp
 Created: 7 Mar 2016 8:03:48pm
 Author:  Martin Hermant

 ==============================================================================
 */


#include "AudioDeviceInNode.h"
#include "NodeBaseUI.h"
#include "AudioDeviceInNodeUI.h"
#include "AudioHelpers.h"
#include "NodeManager.h"

AudioDeviceManager& getAudioDeviceManager();

AudioDeviceInNode::AudioDeviceInNode() :
NodeBase("AudioDeviceIn",NodeType::AudioDeviceInType),
AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType::audioInputNode)
{

  //canHavePresets = false;
  hasMainAudioControl = false;
  
  {
    MessageManagerLock ml;
    getAudioDeviceManager().addChangeListener(this);
  }
  AudioIODevice * ad = getAudioDeviceManager().getCurrentAudioDevice();
  desiredNumAudioInput = addIntParameter("numAudioInput", "desired numAudioInputs (independent of audio settings)",
                                         ad?ad->getActiveInputChannels().countNumberOfSetBits():2, 0, 32);


  lastNumberOfInputs = 0;
  setPreferedNumAudioOutput(desiredNumAudioInput->intValue());
  setPreferedNumAudioInput(0);
  


}

AudioDeviceInNode::~AudioDeviceInNode() {
  MessageManagerLock ml;
  getAudioDeviceManager().removeChangeListener(this);
}

void AudioDeviceInNode::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer & midiMessages) {

  int channelsAvailable = AudioProcessorGraph::AudioGraphIOProcessor::getTotalNumOutputChannels();
  int numSamples = buffer.getNumSamples();



  AudioProcessorGraph::AudioGraphIOProcessor::processBlock(buffer, midiMessages);


  //  totalNumOutputChannels = NodeBase::tot();


  float enabledFactor = enabledParam->boolValue()?1.f:0.f;

  int numChannelsToProcess = jmin(NodeBase::getTotalNumOutputChannels(),channelsAvailable);
  for (int i = 0; i < numChannelsToProcess; i++)
  {
    float newVolume = inMutes[i]->boolValue() ? 0.f : logVolumes[i]*enabledFactor;
    buffer.applyGainRamp(i,0, numSamples, lastVolumes[i], newVolume);
    lastVolumes.set(i, newVolume);

  }
  for(int i = numChannelsToProcess;i<buffer.getNumChannels() ; i++){
    buffer.clear(i,0,numSamples);
  }


}



void AudioDeviceInNode::setParentNodeContainer(NodeContainer * parent){
  NodeBase::setParentNodeContainer(parent);
  jassert(parent == NodeManager::getInstance()->mainContainer);
  AudioGraphIOProcessor::setRateAndBufferSizeDetails(NodeBase::getSampleRate(), NodeBase::getBlockSize());
  updateVolMutes();
}


void AudioDeviceInNode::changeListenerCallback(ChangeBroadcaster*) {
  NodeBase::setRateAndBufferSizeDetails(AudioGraphIOProcessor::getSampleRate(), AudioGraphIOProcessor::getBlockSize());
  updateVolMutes();
}
void AudioDeviceInNode::onContainerParameterChanged(Parameter * p){

  if(p==desiredNumAudioInput){
    setPreferedNumAudioOutput(desiredNumAudioInput->intValue());
  }
  else{
    int foundIdx = volumes.indexOf((FloatParameter*)p);
    if(foundIdx>=0){
      logVolumes.set(foundIdx, float01ToGain(volumes[foundIdx]->floatValue()));
    }
  }

  NodeBase::onContainerParameterChanged(p);

};


void AudioDeviceInNode::updateVolMutes(){

  while(lastNumberOfInputs < desiredNumAudioInput->intValue()){
    addVolMute();
  }
  while (lastNumberOfInputs>desiredNumAudioInput->intValue()) {
    removeVolMute();
  }


}

void AudioDeviceInNode::numChannelsChanged(bool isInput){
  NodeBase::numChannelsChanged(isInput);
  updateVolMutes();
}

void AudioDeviceInNode::addVolMute()
{
  //  const ScopedLock lk (NodeBase::getCallbackLock());
  BoolParameter * p = addBoolParameter(String(inMutes.size() + 1), "Mute if disabled", false);
  p->setCustomShortName(String("mute") + String(inMutes.size() + 1));
  p->invertVisuals = true;
  inMutes.add(p);

  FloatParameter * v = addFloatParameter("volume"+String(volumes.size()), "volume", DB0_FOR_01);
  volumes.add(v);
  lastVolumes.add(0);
  logVolumes.add(float01ToGain(DB0_FOR_01));
  lastNumberOfInputs++;

}

void AudioDeviceInNode::removeVolMute()
{

  if(inMutes.size()==0)return;
  //  const ScopedLock lk (parentNodeContainer->getCallbackLock());
  BoolParameter * b = inMutes[inMutes.size() - 1];
  removeControllable(b);
  inMutes.removeAllInstancesOf(b);

  removeControllable(volumes.getLast());
  lastVolumes.removeLast();
  volumes.removeLast();
  logVolumes.removeLast();
  lastNumberOfInputs--;
}

ConnectableNodeUI * AudioDeviceInNode::createUI() {
  NodeBaseUI * ui = new NodeBaseUI(this,new AudioDeviceInNodeContentUI());
  return ui;

}
