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

#include "DataProcessorGraph.h"
#include "NodeFactory.h"

class NodeManager:
	public ControllableContainer
{

public:
    NodeManager();
    ~NodeManager();

    juce_DeclareSingleton(NodeManager, true);

    AudioProcessorGraph audioGraph;
    DataProcessorGraph dataGraph;

	ScopedPointer<NodeContainer> mainContainer;

	void updateAudioGraph();

    void clear();

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;


    //Listener
    class  NodeManagerListener
    {
    public:
        virtual ~NodeManagerListener() {}

		virtual void managerCleared() {};
    };

    ListenerList<NodeManagerListener> nodeManagerListeners;
    void addNodeManagerListener(NodeManagerListener* newListener) { nodeManagerListeners.add(newListener); }
    void removeNodeManagerListener(NodeManagerListener* listener) { nodeManagerListeners.remove(listener); }


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeManager)


};


#endif  // NODEMANAGER_H_INCLUDED
