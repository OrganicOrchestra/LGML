/*
 ==============================================================================

 VSTNode.h
 Created: 2 Mar 2016 8:37:24pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef VSTNODE_H_INCLUDED
#define VSTNODE_H_INCLUDED


#include "NodeBase.h"

#include "MIDIListener.h"

class VSTNode :
public NodeBase,
public AudioProcessorListener,
public MIDIListener
{

public:
  StringParameter *  identifierString;
  Array<FloatParameter *> VSTParameters;


  VSTNode();
  ~VSTNode();

  class PluginWindowParameters : public ControllableContainer{
  public:
    PluginWindowParameters():ControllableContainer("PluginWindow Parameters")
    {
      x = addFloatParameter("x","x position of plugin window", (float)Random::getSystemRandom().nextInt(500),0.f,1000.f);
      y = addFloatParameter("y","y position of plugin window", (float)Random::getSystemRandom().nextInt (500),0.f,1000.f);
      isDisplayed = addBoolParameter("isDisplayed","is the plugin window displayed",false);
      currentPresetName->isPresettable = false;
    }

    FloatParameter * x;
    FloatParameter * y;
    BoolParameter * isDisplayed;
  };
  PluginWindowParameters pluginWindowParameter;
  void createPluginWindow();
  void closePluginWindow();



  void onContainerParameterChanged(Parameter * p) override;

  void audioProcessorParameterChanged (AudioProcessor* processor,
                                       int parameterIndex,
                                       float newValue) override;

  void audioProcessorChanged (AudioProcessor*) override;
  void loadPresetInternal(PresetManager::Preset * preset)override;
  void savePresetInternal(PresetManager::Preset * preset)override;

  void initParametersFromProcessor(AudioProcessor * p);


  // load state on message thread (some plugin crash if not)

  PresetManager::Preset * presetToLoad;

  //    String getPresetFilter() override;


  bool blockFeedback;

  //Listener
  class VSTNodeListener
  {
  public:
    virtual ~VSTNodeListener() {}
    virtual void newVSTSelected() = 0;
    virtual void midiDeviceChanged() = 0;
  };

  ListenerList<VSTNodeListener> vstNodeListeners;
  void addVSTNodeListener(VSTNodeListener* newListener) { vstNodeListeners.add(newListener); }
  void removeVSTNodeListener(VSTNodeListener* listener) { vstNodeListeners.remove(listener); }


  ConnectableNodeUI * createUI()override;


  // AUDIO
  AudioProcessorEditor * createEditor()override {
    if (innerPlugin)return innerPlugin->createEditor();
    else return nullptr;
  }

  void generatePluginFromDescription(PluginDescription * desc);


  void numChannelsChanged()override;
  void prepareToPlay(double _sampleRate, int _blockSize)override {
    if (innerPlugin) {
      innerPlugin->setProcessingPrecision(singlePrecision);
      innerPlugin->prepareToPlay(_sampleRate, _blockSize); }
  }
  void releaseResources() override { if (innerPlugin) { innerPlugin->releaseResources(); } };
  bool hasEditor() const override { if (innerPlugin) { return innerPlugin->hasEditor(); }return false; };
  void getStateInformation(MemoryBlock & destData)override ;
  void setStateInformation(const void* data, int sizeInBytes)override ;
  void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)override;
  void processBlockBypassed(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)override;
  ScopedPointer<AudioPluginInstance> innerPlugin;


  ///// MIDI
  StringParameter * midiPortNameParam;
  BoolParameter * processWhenBypassed;
  bool bProcessWhenBypassed;
  Trigger * midiActivityTrigger;

  void setCurrentDevice(const String &deviceName) override;
  void handleIncomingMidiMessage(MidiInput* source,
                                 const MidiMessage& message) override;

  MidiMessageCollector messageCollector;
  MidiBuffer incomingMidi;

  int innerPluginTotalNumInputChannels = 0;
  int innerPluginTotalNumOutputChannels = 0;
  int innerPluginMaxCommonChannels = 0;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VSTNode)
};



#endif  // VSTNODE_H_INCLUDED
