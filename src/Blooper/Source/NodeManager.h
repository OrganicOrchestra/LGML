/*
  ==============================================================================

    NodeManager.h
    Created: 2 Mar 2016 8:33:39pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODEMANAGER_H_INCLUDED
#define NODEMANAGER_H_INCLUDED

#include <JuceHeader.h>
#include "NodeBase.h"
#include "juce_audio_processors\juce_audio_processors.h"
#include "DataProcessorGraph.h"

class NodeManager
{
	
public:
	NodeManager();

	void clear();
	int getNumNodes() const noexcept { return nodes.size(); }

	NodeBase* getNode(const int index) const noexcept { return nodes[index]; }
	NodeBase* getNodeForId(const uint32 nodeId) const;
	NodeBase* addNode(uint32 nodeId = 0);
	bool removeNode(uint32 nodeId);


	AudioProcessorGraph * audioGraph;
	DataProcessorGraph * dataGraph;

private:
	ReferenceCountedArray<NodeBase> nodes;
	uint32 lastNodeId;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeManager)
};


#endif  // NODEMANAGER_H_INCLUDED
