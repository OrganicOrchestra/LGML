/*
  ==============================================================================

    ConnectableNode.h
    Created: 18 May 2016 11:33:58pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONNECTABLENODE_H_INCLUDED
#define CONNECTABLENODE_H_INCLUDED

#include "JuceHeader.h"
#include "ControllableContainer.h"
#include "NodeFactory.h"
#include "Data.h"

class ConnectableNode : 
	public ControllableContainer
{
public:
	ConnectableNode(const String &name, NodeType type);
	virtual ~ConnectableNode();

	NodeType type;

	//Interaction
	bool canBeRemovedByUser;
	
	virtual bool hasAudioInputs();
	virtual bool hasAudioOutputs();
	virtual bool hasDataInputs();
	virtual bool hasDataOutputs();

	//Controllables (from ControllableContainer)
	StringParameter * nameParam;
	BoolParameter * enabledParam;
	FloatParameter * xPosition;
	FloatParameter * yPosition;

	void remove(bool askBeforeRemove = false);

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;


	void parameterValueChanged(Parameter * p) override;


	//Listener
	class ConnectableNodeListener
	{
	public:
		virtual ~ConnectableNodeListener() {}
		virtual void askForRemoveNode(ConnectableNode *) {}
		virtual void nodeEnableChanged(ConnectableNode *) {}
	};

	ListenerList<ConnectableNodeListener> nodeListeners;
	void addNodeListener(ConnectableNodeListener* newListener) { nodeListeners.add(newListener); }
	void removeNodeListener(ConnectableNodeListener* listener) { nodeListeners.remove(listener); }




	//AUDIO

	FloatParameter * outputVolume;
	BoolParameter * bypass;
	StringArray inputChannelNames;
	StringArray outputChannelNames;

	virtual AudioProcessorGraph::Node * getAudioNode(bool forInput = true);
	virtual void addToAudioGraph();
	virtual void removeFromAudioGraph();

	void setInputChannelNames(int startChannel, StringArray names);
	void setOutputChannelNames(int startChannel, StringArray names);
	void setInputChannelName(int channelIndex, const String &name);
	void setOutputChannelName(int channelIndex, const String &name);
	String getInputChannelName(int channelIndex);
	String getOutputChannelName(int channelIndex);

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
};


#endif  // CONNECTABLENODE_H_INCLUDED
