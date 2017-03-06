/*
 ==============================================================================

 NodeBase.h
 Created: 2 Mar 2016 8:36:17pm
 Author:  bkupe

 ==============================================================================
 */

/*
 NodeBase is the base class for all Nodes
 it contains NodeAudioProcessor and/or NodeBase::NodeDataProcessor


 */
#ifndef NODEBASE_H_INCLUDED
#define NODEBASE_H_INCLUDED
#pragma once


#include "ConnectableNode.h"
#include "AudioHelpers.h"

class ConnectableNodeUI;


class NodeBase :
public ConnectableNode,
public ReferenceCountedObject,
public juce::AudioProcessor, private Timer, //Audio
public Data::DataListener //Data
{

public:
  NodeBase(const String &name = "[NodeBase]", NodeType type = UNKNOWN_TYPE, bool _hasMainAudioControl = true);
  virtual ~NodeBase();


  virtual bool hasAudioInputs() override;
  virtual bool hasAudioOutputs() override;
  virtual bool hasDataInputs() override;
  virtual bool hasDataOutputs() override;





  //  TODO:  this should not be implemented in Node to avoid overriding this method
  void onContainerParameterChanged(Parameter * p) override;
  void onContainerParameterChangedAsync(Parameter *,const var & /*value*/)override{};


  virtual void clear() override;
  // can be oerriden to react to clear
  virtual void clearInternal() {};

  var getJSONData() override;
  void loadJSONDataInternal(var data) override;


  //ui
  virtual ConnectableNodeUI *  createUI() override;

  virtual const String getName() const override
  {
    return nameParam->stringValue();
  }



  //AUDIO PROCESSOR



  virtual bool setPreferedNumAudioInput(int num);
  virtual bool setPreferedNumAudioOutput(int num);
  // this will be called with audio locked so that one can safely update internal variables used by processBlockInternal
  void numChannelsChanged() override{numChannelsChanged(true);numChannelsChanged(false);};
  virtual void numChannelsChanged(bool /*isInput*/) {};


  virtual void prepareToPlay(double, int) override {};
  virtual void releaseResources() override {};

  //bool silenceInProducesSilenceOut() const override { return false; }

  virtual AudioProcessorEditor* createEditor() override { return nullptr; }
  virtual bool hasEditor() const override { return false; }



  // dumb overrides from JUCE AudioProcessor :  MIDI
  int getNumPrograms() override { return 0; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  const String getProgramName(int) override { return "NoProgram"; }
  void changeProgramName(int, const String&) override {};
  double getTailLengthSeconds() const override { return 0; }
  bool acceptsMidi() const override { return false; }
  bool producesMidi() const override { return false; }



  // save procedures from host
  virtual void getStateInformation(juce::MemoryBlock&) override {};
  virtual void setStateInformation(const void*, int) override {};

  virtual void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
  virtual void processBlockInternal(AudioBuffer<float>& /*buffer*/ , MidiBuffer& /*midiMessage*/ ) {};
  virtual void processBlockBypassed(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

  int totalNumInputChannels = 0;
  int totalNumOutputChannels = 0;
  int maxCommonIOChannels = 0;
  //RMS
  const float alphaRMS = 0.05f;
  const int samplesBeforeRMSUpdate = 512;
  int curSamplesForRMSInUpdate = 0;
  int curSamplesForRMSOutUpdate = 0;
  float globalRMSValueIn ;
  float globalRMSValueOut ;

  Array<float> rmsValuesIn;
  Array<float> rmsValuesOut;

  //Listener are called from non audio thread
  void timerCallback() override;

  bool wasSuspended;
  SmoothedValue<double> logVolume;

  float lastVolume;

  //DATA
  virtual Data* getInputData(int dataIndex) override;
  virtual Data* getOutputData(int dataIndex) override;


  typedef Data::DataType DataType;
  typedef Data::DataElement DataElement;
  OwnedArray<Data> inputDatas;
  OwnedArray<Data> outputDatas;
  CriticalSection numDataIOLock;

  Data * addInputData(const String &name, DataType type);
  Data * addOutputData(const String &name, DataType type);


  bool removeInputData(const String &name);
  bool removeOutputData(const String &name);

  void removeAllInputDatas();
  void removeAllOutputDatas();

  virtual void updateOutputData(String &dataName, const float &value1, const float &value2 = 0, const float &value3 = 0);


  int getTotalNumInputData() override;
  int getTotalNumOutputData() override;

  StringArray getInputDataInfos() override;
  StringArray getOutputDataInfos() override;

  Data::DataType getInputDataType(const String &dataName, const String &elementName) override;
  Data::DataType getOutputDataType(const String &dataName, const String &elementName) override;


  Data * getOutputDataByName(const String &dataName) override;
  Data * getInputDataByName(const String &dataName) override;

  virtual void dataChanged(Data *) override;

  virtual void processInputDataChanged(Data *) {} // to be overriden by child classes
  virtual void processOutputDataUpdated(Data *) {} // to be overriden by child classes
private:
  WeakReference<NodeBase>::Master masterReference;
  friend class WeakReference<NodeBase>;

  FadeInOut dryWetFader,muteFader;

  double lastDryVolume;
  bool wasEnabled;
  AudioBuffer<float> crossFadeBuffer;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeBase)
  
};

#endif  // NODEBASE_H_INCLUDED
