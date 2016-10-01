/*
  ==============================================================================

    ConnectableNode.h
    Created: 18 May 2016 11:33:58pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONNECTABLENODE_H_INCLUDED
#define CONNECTABLENODE_H_INCLUDED

#include "ControllableContainer.h"
#include "NodeFactory.h"
#include "Data.h"

class NodeContainer;

class ConnectableNodeUI;

class ConnectableNode :
	public ControllableContainer
{
public:
	ConnectableNode(const String &name, NodeType type, bool _hasMainAudioControl = true);
	virtual ~ConnectableNode();

	NodeType type;

	NodeContainer * parentNodeContainer;
  
	virtual void setParentNodeContainer(NodeContainer * _parentNodeContainer);
  AudioProcessorGraph::Node * audioNode;
  AudioProcessor * getAudioProcessor();

	//Interaction
	bool canBeRemovedByUser;
	bool userCanAccessInputs;
	bool userCanAccessOutputs;

	virtual bool hasAudioInputs();
	virtual bool hasAudioOutputs();
	virtual bool hasDataInputs();
	virtual bool hasDataOutputs();

	//Controllables (from ControllableContainer)
	StringParameter * nameParam;
    StringParameter * descriptionParam;
	BoolParameter * enabledParam;

	//ui params
	FloatParameter * xPosition;
	FloatParameter * yPosition;
	BoolParameter * miniMode;

	void remove(bool askBeforeRemove = false);
	virtual void clear();

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;


	void onContainerParameterChanged(Parameter * p) override;

	//ui
	virtual ConnectableNodeUI *  createUI();

	//Listener
	class ConnectableNodeListener
	{
	public:
		virtual ~ConnectableNodeListener() {}
		virtual void askForRemoveNode(ConnectableNode *) {}
		virtual void nodeParameterChanged(ConnectableNode *,Parameter *) {}

    virtual void numAudioInputChanged(ConnectableNode *, int /*newNumInput*/) {};
    virtual void numAudioOutputChanged(ConnectableNode *, int /*newNumOutput*/) {};
    virtual void numDataInputChanged(ConnectableNode *, int /*newNumInput*/) {};
    virtual void numDataOutputChanged(ConnectableNode *, int /*newNumOutput*/) {};

    virtual void audioInputAdded(ConnectableNode *, int /*channel*/) {}
    virtual void audioInputRemoved(ConnectableNode *, int /*channel*/) {}
    virtual void audioOutputAdded(ConnectableNode *, int /*channel*/) {}
    virtual void audioOutputRemoved(ConnectableNode *, int /*channel*/) {}


    virtual void dataInputAdded(ConnectableNode *, Data *) {}
    virtual void dataInputRemoved(ConnectableNode *, Data *) {}
    virtual void dataOutputAdded(ConnectableNode *, Data *) {}
    virtual void dataOutputRemoved(ConnectableNode *, Data *) {}
    virtual void nodeInputDataChanged(ConnectableNode *, Data *) {}
    virtual void nodeOutputDataUpdated(ConnectableNode *, Data *) {}

  };
  
  ListenerList<ConnectableNodeListener> nodeListeners;
	void addConnectableNodeListener(ConnectableNodeListener* newListener) { nodeListeners.add(newListener); }
	void removeConnectableNodeListener(ConnectableNodeListener* listener) { nodeListeners.remove(listener); }




	//AUDIO
	bool hasMainAudioControl;

	FloatParameter * outputVolume;
	//BoolParameter * bypass;
	StringArray inputChannelNames;
	StringArray outputChannelNames;

	virtual AudioProcessorGraph::Node * getAudioNode(bool forInput = true);
	virtual void addToAudioGraph(AudioProcessorGraph*);
	virtual void removeFromAudioGraph();

	void setInputChannelNames(int startChannel, StringArray names);
	void setOutputChannelNames(int startChannel, StringArray names);
	void setInputChannelName(int channelIndex, const String &name);
	void setOutputChannelName(int channelIndex, const String &name);
	String getInputChannelName(int channelIndex);
	String getOutputChannelName(int channelIndex);

	class  RMSListener
	{
	public:
		/** Destructor. */
		virtual ~RMSListener() {}
		virtual void RMSChanged(ConnectableNode * node, float rmsInValue, float rmsOutValue) = 0;
	};


    ListenerList<RMSListener> rmsListeners;
    void addRMSListener(RMSListener* newListener) { rmsListeners.add(newListener); }
    void removeRMSListener(RMSListener* listener) { rmsListeners.remove(listener); }

    class RMSChannelListener{
    public:
        virtual ~RMSChannelListener(){}
        virtual void channelRMSInChanged(ConnectableNode * node, float rmsInValue, int channel) = 0;
        virtual void channelRMSOutChanged(ConnectableNode * node, float rmsOutValue, int channel) = 0;
    };


    ListenerList<RMSChannelListener> rmsChannelListeners;
    void addRMSChannelListener(RMSChannelListener* newListener) { rmsChannelListeners.add(newListener); }
    void removeRMSChannelListener(RMSChannelListener* listener) { rmsChannelListeners.remove(listener); }

    String getPresetFilter()override;

	//DATA
	virtual Data* getInputData(int dataIndex);
	virtual Data* getOutputData(int dataIndex);

	virtual int getTotalNumInputData();
	virtual int getTotalNumOutputData();

	virtual StringArray getInputDataInfos();
	virtual StringArray getOutputDataInfos();

	virtual Data::DataType getInputDataType(const String &dataName, const String &elementName);
	virtual Data::DataType getOutputDataType(const String &dataName, const String &elementName);

	virtual Data * getOutputDataByName(const String &dataName);
	virtual Data * getInputDataByName(const String &dataName);

private:
    WeakReference<ConnectableNode>::Master masterReference;
    friend class WeakReference<ConnectableNode>;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectableNode)
};


#endif  // CONNECTABLENODE_H_INCLUDED
