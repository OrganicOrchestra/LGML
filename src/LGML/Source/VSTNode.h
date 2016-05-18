/*
 ==============================================================================

 VSTNode.h
 Created: 2 Mar 2016 8:37:24pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef VSTNODE_H_INCLUDED
#define VSTNODE_H_INCLUDED


#include "VSTManager.h"
#include "NodeBase.h"

#include "PluginWindow.h"
#include "TimeManager.h"
#include "MIDIListener.h"

AudioDeviceManager& getAudioDeviceManager();


class VSTNode :
public NodeBase,
public AudioProcessorListener,
public MIDIListener
{

public:
    StringParameter *  identifierString;
    Array<FloatParameter *> VSTParameters;


    VSTNode(uint32 nodeId);
    ~VSTNode();

    class PluginWindowParameters : public ControllableContainer{
    public:
        PluginWindowParameters():ControllableContainer("PluginWindow Parameters")
        {
            x = addFloatParameter("x","x position of plugin window", (float)Random::getSystemRandom().nextInt(500),0.f,1000.f);
            y = addFloatParameter("y","y position of plugin window", (float)Random::getSystemRandom().nextInt (500),0.f,1000.f);
            isDisplayed = addBoolParameter("isDisplayed","is the plugin window displayed",false);
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

    void audioProcessorChanged (AudioProcessor*) override{};



    void initParametersFromProcessor(AudioProcessor * p);


    String getPresetFilter() override;


    bool blockFeedback;

    //Listener
    class VSTNodeListener
    {
    public:
        virtual ~VSTNodeListener() {}
        virtual void newVSTSelected() = 0;

    };

    ListenerList<VSTNodeListener> vstNodeListeners;
    void addVSTNodeListener(VSTNodeListener* newListener) { vstNodeListeners.add(newListener); }
    void removeVSTNodeListener(VSTNodeListener* listener) { vstNodeListeners.remove(listener); }


    NodeBaseUI * createUI()override;




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
    void getStateInformation(MemoryBlock & destData)override { if (innerPlugin) { innerPlugin->getStateInformation(destData); }; }
    void setStateInformation(const void* data, int sizeInBytes)override { if (innerPlugin) { innerPlugin->setStateInformation(data, sizeInBytes); }; };
    void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)override;;

    ScopedPointer<AudioPluginInstance> innerPlugin;


    ///// MIDI

    void handleIncomingMidiMessage(MidiInput* source,
                                   const MidiMessage& message) override;

    MidiMessageCollector messageCollector;
        MidiBuffer incomingMidi;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VSTNode)
};



#endif  // VSTNODE_H_INCLUDED
