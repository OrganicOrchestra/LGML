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
#include "NodeConnection.h"
#include "NodeFactory.h"
#include "NodeContainer.h"

class NodeManager: 
	public ControllableContainer
{

public:
    NodeManager();
    ~NodeManager();

    juce_DeclareSingleton(NodeManager, true);
	
    AudioProcessorGraph audioGraph;
    DataProcessorGraph dataGraph;

	NodeContainer mainContainer;

	void updateAudioGraph();

    void clear();
   
	var getJSONData() override;
	void loadJSONDataInternal(var data) override;
	/*
    //Listener
    class  Listener
    {
    public:
        virtual ~Listener() {}

        virtual void nodeAdded(NodeBase *) = 0;
        virtual void nodeRemoved(NodeBase *) = 0;

        virtual void connectionAdded(NodeConnection *) = 0;
        virtual void connectionRemoved(NodeConnection *) = 0;
    };

    ListenerList<Listener> listeners;
    void addNodeManagerListener(Listener* newListener) { listeners.add(newListener); }
    void removeNodeManagerListener(Listener* listener) { listeners.remove(listener); }
	*/

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeManager)

	
};


#endif  // NODEMANAGER_H_INCLUDED
