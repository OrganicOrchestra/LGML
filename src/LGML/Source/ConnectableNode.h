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

class ConnectableNode : 
	public ControllableContainer
{
public:
	ConnectableNode(const String &name);
	virtual ~ConnectableNode();

	NodeType type;

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

	
};


#endif  // CONNECTABLENODE_H_INCLUDED
