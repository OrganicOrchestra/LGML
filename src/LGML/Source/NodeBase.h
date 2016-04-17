/*
 ==============================================================================

 NodeBase.h
 Created: 2 Mar 2016 8:36:17pm
 Author:  bkupe

 ==============================================================================
 */

/*
 NodeBase is the base class for all Nodes
 it contains NodeBase::NodeAudioProcessor and/or NodeBase::NodeDataProcessor


 */
#ifndef NODEBASE_H_INCLUDED
#define NODEBASE_H_INCLUDED

#include "JuceHeader.h"
#include "DataProcessor.h"
#include "ControllableContainer.h"

class NodeBaseUI;
class NodeManager;



class NodeBase : public ReferenceCountedObject, public DataProcessor::Listener, public ControllableContainer
{

public:
    class NodeAudioProcessor : public juce::AudioProcessor,public AsyncUpdater
    {
    public:
        NodeAudioProcessor() :AudioProcessor(){};

        virtual const String getName() const override { return "NodeBaseProcessor"; };

        virtual void prepareToPlay(double ,int) override{};
        virtual void releaseResources() override {};

        bool silenceInProducesSilenceOut() const override { return false; }

        virtual AudioProcessorEditor* createEditor() override {return nullptr ;}
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


        virtual void processBlock(AudioBuffer<float>& buffer,MidiBuffer& midiMessages) override ;
        virtual void processBlockInternal(AudioBuffer<float>& buffer,MidiBuffer& midiMessages) = 0;


        void updateRMS(AudioBuffer<float>& buffer);
        float alphaRMS = 0.05f;
        float rmsValue = 0.f;
        const int samplesBeforeRMSUpdate = 512;
        int curSamplesForRMSUpdate = 0;

        //Listener are called from non audio thread
        void handleAsyncUpdate() override{listeners.call(&Listener::RMSChanged,rmsValue);}


		
        class  Listener
        {
        public:
            /** Destructor. */
            virtual ~Listener() {}
            virtual void RMSChanged(float ) = 0;

        };

        ListenerList<Listener> listeners;
        void addRMSListener(Listener* newListener) { listeners.add(newListener); }
        void removeRMSListener(Listener* listener) { listeners.remove(listener); }

        ScopedPointer<AudioProcessor> audioProcessorImpl;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeAudioProcessor)
    };


    class NodeDataProcessor : public DataProcessor
    {
    public:
        NodeDataProcessor() {};

        // Inherited via DataProcessor
        virtual void receiveData(const Data * incomingData, const String &destDataName, const String &destElementName = "", const String &sourceElementName = "") override;
        virtual void sendData(const Data * outgoingData, const String &sourceElementName = "") override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeDataProcessor)

    };



public:
    NodeBase(NodeManager * nodeManager,uint32 nodeId, const String &name = "[NodeBase]", NodeBase::NodeAudioProcessor * audioProcessor = nullptr, NodeBase::NodeDataProcessor * dataProcessor = nullptr);
    virtual ~NodeBase();

    uint32 nodeId;
    NodeManager * nodeManager;

    // owned by audio Graph in a refference Counted Array
    NodeAudioProcessor *  audioProcessor;
    //owned by data Graph in a reference counted array
    ScopedPointer<NodeDataProcessor> dataProcessor;

    bool hasAudioInputs;
    bool hasAudioOutputs;
    bool hasDataInputs;
    bool hasDataOutputs;

    void checkInputsAndOutputs();

    void remove();


    //Controllables (from ControllableContainer)
    StringParameter * nameParam;
    BoolParameter * enabledParam;
    FloatParameter * xPosition;
    FloatParameter * yPosition;

    virtual void parameterValueChanged(Parameter * p) override;

    //audio
    void addToAudioGraphIfNeeded();
    void removeFromAudioGraphIfNeeded();


    //ui
    virtual NodeBaseUI *  createUI() {DBG("No implementation in child node class !");jassert(false);return nullptr;}

    // Inherited via DataProcessor::Listener
    virtual void inputAdded(DataProcessor::Data *) override;
    virtual void inputRemoved(DataProcessor::Data *) override;
    virtual void outputAdded(DataProcessor::Data *) override;
    virtual void ouputRemoved(DataProcessor::Data *) override;


	var getJSONData();
	void loadJSONData(var data);


    //Listener
    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void askForRemoveNode(NodeBase *) = 0;

    };

    ListenerList<Listener> listeners;
    void addRemoveNodeListener(Listener* newListener) { listeners.add(newListener); }
    void removeRemoveNodeListener(Listener* listener) { listeners.remove(listener); }

// keeps type info from NodeFactory
    int nodeTypeEnum;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeBase)





};

#endif  // NODEBASE_H_INCLUDED
