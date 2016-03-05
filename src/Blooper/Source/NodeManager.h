/*
  ==============================================================================

    NodeManager.h
    Created: 2 Mar 2016 8:33:39pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODEMANAGER_H_INCLUDED
#define NODEMANAGER_H_INCLUDED
/*
Node Manager Contain all Node and synchronize building of audioGraph (AudioProcessorGraph) and DataGraph (DataProcessorGraph)

*/
#include "JuceHeader.h"

#include "juce_audio_processors\juce_audio_processors.h"
#include "DataProcessorGraph.h"

#include "NodeFactory.h"

class NodeManager: public NodeBase::Listener
{
	
public:
	NodeManager();
	~NodeManager();


	NodeFactory nodeFactory;


	bool removeNode(uint32 nodeId);
	AudioProcessorGraph audioGraph;
	DataProcessorGraph dataGraph;


	void clear();
	int getNumNodes() const noexcept { return nodes.size(); }

	NodeBase* getNode(const int index) const noexcept { return nodes[index]; }
	NodeBase* getNodeForId(const uint32 nodeId) const;
	NodeBase* addNode(NodeFactory::NodeType nodeType, uint32 nodeId = 0);
	

	//Listener
	class  Listener
	{
	public:
		/** Destructor. */
		virtual ~Listener() {}

		virtual void nodeAdded(NodeBase *) = 0;
		virtual void nodeRemoved(NodeBase *) = 0;

		virtual void audioConnectionAdded(AudioProcessorGraph::Connection *) = 0;
		virtual void audioConnectionRemoved(AudioProcessorGraph::Connection *) = 0;

		virtual void dataConnectionAdded(DataProcessorGraph::Connection *) = 0;
		virtual void dataConnectionRemoved(DataProcessorGraph::Connection *) = 0;
	};

	ListenerList<Listener> listeners;

	void addListener(Listener* newListener);

	/** Removes a previously-registered button listener
	@see addListener
	*/
	void removeListener(Listener* listener);

private: 
	ReferenceCountedArray<NodeBase> nodes;
	uint32 lastNodeId;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeManager)

		// Inherited via Listener
		virtual void askForRemoveNode(NodeBase *) override;
};


#endif  // NODEMANAGER_H_INCLUDED
