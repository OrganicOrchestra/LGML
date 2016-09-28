/*
 ==============================================================================

 NodeBase.cpp
 Created: 2 Mar 2016 8:36:17pm
 Author:  bkupe

 ==============================================================================
 */

#include "NodeBase.h"
#include "NodeManager.h"
#include "TimeManager.h"

#include "AudioHelpers.h"

NodeBase::NodeBase(const String &name,NodeType _type, bool _hasMainAudioControl) :
ConnectableNode(name,_type,_hasMainAudioControl),
enableFader(5000,5000,false,1),
lastDryVolume(0)

{
  
  logVolume = float01ToGain(DB0_FOR_01);

  lastVolume = hasMainAudioControl ? outputVolume->floatValue() : 0;
  enableFader.startFadeIn();

  for (int i = 0; i < 2; i++) rmsValuesIn.add(0);
  for (int i = 0; i < 2; i++) rmsValuesIn.add(0);
  startTimerHz(30);
}


NodeBase::~NodeBase()
{
  stopTimer();
  NodeBase::masterReference.clear();
  clear();
}


bool NodeBase::hasAudioInputs()
{
  //to override
  return getTotalNumInputChannels() > 0;
}

bool NodeBase::hasAudioOutputs()
{
  //to override
  return getTotalNumOutputChannels() > 0;
}

bool NodeBase::hasDataInputs()
{
  //to override
  return getTotalNumInputData()>0;
}

bool NodeBase::hasDataOutputs()
{
  //to override
  return getTotalNumOutputData()>0;
}


void NodeBase::onContainerParameterChanged(Parameter * p)
{
  ConnectableNode::onContainerParameterChanged(p);

  if(p==outputVolume){
    logVolume = float01ToGain(outputVolume->floatValue());
  }

  //ENABLE PARAM ACT AS A BYPASS

  if (p == enabledParam)
  {
    if(enabledParam->boolValue()){enableFader.startFadeIn();}
    else {enableFader.startFadeOut();}
  }

}

void NodeBase::clear()
{
  clearInternal();

  //Data
  inputDatas.clear();
  outputDatas.clear();
  stopTimer();

  //removeFromAudioGraph();
}




//Save / Load

var NodeBase::getJSONData()
{
  var data = ConnectableNode::getJSONData();

  MemoryBlock m;

  // TODO we could implement that for all node objects to be able to save any kind of custom data
  getStateInformation(m);

  if (m.getSize()) {
    var audioProcessorData(new DynamicObject());
    audioProcessorData.getDynamicObject()->setProperty("state", m.toBase64Encoding());
    data.getDynamicObject()->setProperty("audioProcessor", audioProcessorData);
  }

  return data;
}

void NodeBase::loadJSONDataInternal(var data)
{
  ConnectableNode::loadJSONDataInternal(data);

  var audioProcessorData = data.getProperty("audioProcessor", var());
  String audioProcessorStateData = audioProcessorData.getProperty("state",var());

  MemoryBlock m;
  m.fromBase64Encoding(audioProcessorStateData);
  setStateInformation(m.getData(), (int)m.getSize());
}

//ui

ConnectableNodeUI * NodeBase::createUI() {
  DBG("No implementation in child node class !");
  jassert(false);
  return nullptr;
}




/////////////////////////////////////// AUDIO



void NodeBase::processBlock(AudioBuffer<float>& buffer,
                            MidiBuffer& midiMessages) {

  // be sure to delete input if we are not enabled and a random buffer enters
  // juceAudioGraph seems to use the fact that we shouldn't process audio to pass others
  int numSample = buffer.getNumSamples();
  for(int i = getTotalNumInputChannels();i < buffer.getNumChannels() ; i++){
    buffer.clear(i,0,numSample);
  }
  if (rmsListeners.size() || rmsChannelListeners.size()) {
    curSamplesForRMSInUpdate += numSample;
    if (curSamplesForRMSInUpdate >= samplesBeforeRMSUpdate) {
      updateRMS(true,buffer, globalRMSValueIn,rmsValuesIn,rmsChannelListeners.size()==0);
      curSamplesForRMSInUpdate = 0;
    }
  }




  const double fadeValue = enableFader.getCurrentFade();
  enableFader.incrementFade(numSample);
  
  // on disable
  if(wasEnabled && fadeValue==0 ){

//    suspendProcessing(true);
    wasEnabled = false;
  }
  // on Enable
  if(!wasEnabled && fadeValue>0 ){
//    suspendProcessing(false);
    wasEnabled = true;
  }

  if (!isSuspended())
  {
    double curVolume = logVolume*fadeValue;
    double curDryVolume = logVolume*(1.0-fadeValue);
    if (fadeValue>0 ){
      if(fadeValue!=1){crossFadeBuffer.makeCopyOf(buffer);}
      processBlockInternal(buffer, midiMessages);
      if(fadeValue!=1 || hasMainAudioControl){
        buffer.applyGainRamp(0, numSample, lastVolume, curVolume);
      }
      if(fadeValue!=1){
        for(int i = 0 ; i < getTotalNumOutputChannels() ; i++){
          buffer.addFromWithRamp(i, 0, crossFadeBuffer.getReadPointer(i), numSample, lastDryVolume,curDryVolume);
        }
      }
    }
    lastVolume = curVolume;
    lastDryVolume = curDryVolume;

  }
  else{
    DBG("suspended");
  }


  // be sure to delete out if we are not enabled and a random buffer enters
  // juceAudioGraph seems to use the fact that we shouldn't process audio to pass others
  for(int i = getTotalNumOutputChannels();i < buffer.getNumChannels() ; i++){
    buffer.clear(i,0,numSample);
  }

  if (rmsListeners.size() || rmsChannelListeners.size()) {
    curSamplesForRMSOutUpdate += numSample;
    if (curSamplesForRMSOutUpdate >= samplesBeforeRMSUpdate) {
      updateRMS(false,buffer, globalRMSValueOut,rmsValuesOut,rmsChannelListeners.size()==0);
      curSamplesForRMSOutUpdate = 0;
    }
  }


};

bool NodeBase::setPreferedNumAudioInput(int num) {

  int oldNumChannels = getTotalNumInputChannels();


  NodeBase::setPlayConfigDetails(num, getTotalNumOutputChannels(),
                                 getSampleRate(),
                                 getBlockSize());



  if (parentNodeContainer != nullptr){
    parentNodeContainer->updateAudioGraph();
  }
  rmsValuesIn.clear();
  for (int i = 0; i < getTotalNumInputChannels(); i++) rmsValuesIn.add(0);

  int newNum = getTotalNumInputChannels();
  if (newNum > oldNumChannels)
  {
    for (int i = oldNumChannels; i < newNum; i++)
    {
      nodeListeners.call(&ConnectableNodeListener::audioInputAdded, this, i);
    }
  }
  else
  {
    for (int i = oldNumChannels - 1; i >= newNum; i--)
    {
      nodeListeners.call(&ConnectableNodeListener::audioInputRemoved, this, i);
    }
  }

  nodeListeners.call(&ConnectableNodeListener::numAudioInputChanged, this,num);

  return true;
}
bool NodeBase::setPreferedNumAudioOutput(int num) {

  int oldNumChannels = getTotalNumOutputChannels();

  setPlayConfigDetails(getTotalNumInputChannels(), num,
                       getSampleRate(),
                       getBlockSize());



if(parentNodeContainer)
   parentNodeContainer->updateAudioGraph();


  rmsValuesOut.clear();
  for (int i = 0; i < getTotalNumOutputChannels(); i++) rmsValuesOut.add(0);

  int newNum = getTotalNumOutputChannels();
  if (newNum > oldNumChannels)
  {
    for (int i = oldNumChannels; i < newNum; i++)
    {
      nodeListeners.call(&ConnectableNodeListener::audioOutputAdded, this, i);
    }
  }else
  {
    for (int i = oldNumChannels-1; i >= newNum; i--)
    {
      nodeListeners.call(&ConnectableNodeListener::audioOutputRemoved, this, i);
    }
  }

  nodeListeners.call(&ConnectableNodeListener::numAudioOutputChanged,this,num);

  return true;
}

void NodeBase::updateRMS(bool isInput,const AudioBuffer<float>& buffer, float &targetRmsValue, Array<float> &targetRMSChannelValues,bool skipChannelComputation) {
  int numSamples = buffer.getNumSamples();
  int numChannels = jmin((isInput?getTotalNumInputChannels():getTotalNumOutputChannels()),buffer.getNumChannels());
  if(targetRMSChannelValues.size()!=numChannels)
    targetRMSChannelValues.resize(numChannels);

#ifdef HIGH_ACCURACY_RMS
  for (int i = numSamples - 64; i >= 0; i -= 64) {
    rmsValue += alphaRMS * (buffer.getRMSLevel(0, i, 64) - rmsValue);
  }
#else
  // faster implementation taken from juce Device Settings input meter

  float globalS = 0;

  // @ben we need that (window of 64 sample cannot describe any accurate RMS level alone thus decay factor)
  const double decayFactor = 0.95;
  const float lowThresh = 0.0001f;

  if(skipChannelComputation){
    for (int i = numChannels - 1; i >= 0; --i)
    {

      float s = 0;
      Range<float> minMax = FloatVectorOperations::findMinAndMax(buffer.getReadPointer(i), numSamples);
      s = jmax(s,-minMax.getStart());
      s = jmax(s,minMax.getEnd());
      globalS = jmax(s, globalS);
    }
  }
  else{
    for (int i = numChannels - 1; i >= 0; --i)
    {

      float s = 0;
      Range<float> minMax = FloatVectorOperations::findMinAndMax(buffer.getReadPointer(i), numSamples);
      s = jmax(s,-minMax.getStart());
      s = jmax(s,minMax.getEnd());
      targetRMSChannelValues.set(i, (s>targetRMSChannelValues.getUnchecked(i))?s:
                                 s>lowThresh?targetRMSChannelValues.getUnchecked(i)*(float)decayFactor:
                                 0);

      globalS = jmax(s, globalS);
    }
  }

  if (globalS > targetRmsValue)
    targetRmsValue = globalS;
  else if (targetRmsValue > lowThresh)
    targetRmsValue *= (float)decayFactor;
  else
    targetRmsValue = 0;


#endif


}


void NodeBase::timerCallback()
{
  ConnectableNode::rmsListeners.call(&ConnectableNode::RMSListener::RMSChanged, this, globalRMSValueIn, globalRMSValueOut);
  for (int i = 0; i < getTotalNumInputChannels(); i++)
  {
    ConnectableNode::rmsChannelListeners.call(&ConnectableNode::RMSChannelListener::channelRMSInChanged, this, rmsValuesIn[i], i);
  }

  for (int i = 0; i < getTotalNumOutputChannels(); i++)
  {
    ConnectableNode::rmsChannelListeners.call(&ConnectableNode::RMSChannelListener::channelRMSOutChanged, this, rmsValuesOut[i], i);
  }
}

//////////////////////////////////   DATA

Data * NodeBase::getInputData(int dataIndex)
{
  return inputDatas[dataIndex];
}


Data * NodeBase::getOutputData(int dataIndex)
{
  return outputDatas[dataIndex];
}


Data * NodeBase::addInputData(const String & name, Data::DataType dataType)
{
  Data *d = new Data(this, name, dataType);
  inputDatas.add(d);

  d->addDataListener(this);

  nodeListeners.call(&ConnectableNodeListener::dataInputAdded, this, d);
  nodeListeners.call(&ConnectableNodeListener::numDataInputChanged, this, inputDatas.size());
  return d;
}

Data * NodeBase::addOutputData(const String & name, DataType dataType)
{
  Data * d = new Data(this, name, dataType);
  outputDatas.add(d);

  nodeListeners.call(&ConnectableNodeListener::dataOutputAdded, this, d);
  nodeListeners.call(&ConnectableNodeListener::numDataOutputChanged, this, inputDatas.size());
  return d;
}

void NodeBase::removeInputData(const String & name)
{
  Data * d = getInputDataByName(name);
  if (d == nullptr) return;

  inputDatas.removeObject(d, false);
  nodeListeners.call(&ConnectableNodeListener::dataInputRemoved, this, d);
  nodeListeners.call(&ConnectableNodeListener::numDataInputChanged, this, inputDatas.size());
  delete d;
}

void NodeBase::removeOutputData(const String & name)
{
  Data * d = getOutputDataByName(name);
  if (d == nullptr) return;

  outputDatas.removeObject(d, false);
  nodeListeners.call(&ConnectableNodeListener::dataOutputRemoved, this, d);
  nodeListeners.call(&ConnectableNodeListener::numDataOutputChanged, this, inputDatas.size());
  delete d;
}

void NodeBase::updateOutputData(String & dataName, const float & value1, const float & value2, const float & value3)
{
  Data * d = getOutputDataByName(dataName);
  if (d != nullptr) d->update(value1, value2, value3);
}

int NodeBase::getTotalNumInputData() {
  return inputDatas.size();
}

int NodeBase::getTotalNumOutputData() {
  return outputDatas.size();
}

StringArray NodeBase::getInputDataInfos()
{
  StringArray dataInfos;
  for (auto &d : inputDatas) dataInfos.add(d->name + " (" + d->getTypeString() + ")");
  return dataInfos;
}

StringArray NodeBase::getOutputDataInfos()
{
  StringArray dataInfos;
  for (auto &d : outputDatas) dataInfos.add(d->name + " (" + d->getTypeString() + ")");
  return dataInfos;
}

Data::DataType NodeBase::getInputDataType(const String &dataName, const String &elementName)
{
  for (int i = inputDatas.size(); --i >= 0;)
  {
    Data* d = inputDatas.getUnchecked(i);

    if (d->name == dataName)
    {
      if (elementName.isEmpty())
      {
        return d->type;
      }
      else
      {
        DataElement * e = d->getElement(elementName);
        if (e == nullptr) return DataType::Unknown;
        return e->type;
      }
    }
  }

  return DataType::Unknown;
}

Data::DataType NodeBase::getOutputDataType(const String &dataName, const String &elementName)
{
  for (int i = outputDatas.size(); --i >= 0;)
  {
    Data* d = outputDatas.getUnchecked(i);

    if (d->name == dataName)
    {
      if (elementName.isEmpty())
      {
        return d->type;
      }
      else
      {
        DataElement * e = d->getElement(elementName);
        if (e == nullptr) return DataType::Unknown;
        return e->type;
      }
    }
  }

  return DataType::Unknown;

}

Data * NodeBase::getOutputDataByName(const String & dataName)
{
  for (auto &d : outputDatas)
  {
    if (d->name == dataName) return d;
  }

  return nullptr;
}

Data * NodeBase::getInputDataByName(const String & dataName)
{
  for (auto &d : inputDatas)
  {
    if (d->name == dataName) return d;
  }

  return nullptr;
}

void NodeBase::dataChanged(Data * d)
{
  if (enabledParam->boolValue()) {
    processInputDataChanged(d);
  }
}

void NodeBase::processInputDataChanged(Data *)
{
}
