/*
  ==============================================================================

    NodeConnectionUI.h
    Created: 4 Mar 2016 3:19:24pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODECONNECTIONUI_H_INCLUDED
#define NODECONNECTIONUI_H_INCLUDED


#include "NodeBaseUI.h"
#include "NodeManagerUI.h"

//==============================================================================
/*


*/

class NodeConnectionUI : 
	public InspectableComponent, 
	public ComponentListener,
	public NodeConnection::Listener
{
public:
    typedef ConnectorComponent Connector;

    NodeConnectionUI(NodeConnection * connection = nullptr, Connector* sourceConnector = nullptr, Connector * destConnector = nullptr);
    ~NodeConnectionUI();

    NodeConnection * connection;


    Connector * sourceConnector;
    Connector * destConnector;
    Connector * candidateDropConnector;

    void setSourceConnector(Connector * c);
    void setDestConnector(Connector * c);


    Path hitPath;

    void paint(Graphics&)override;
    void resized()override;
    void componentParentHierarchyChanged(Component&) override { removeComponentListener(this); updateBoundsFromNodes(); }

    void updateBoundsFromNodes();
    virtual bool hitTest(int x, int y) override{ return hitPath.contains((float)x, (float)y); }

    //interaction
    void mouseDown(const MouseEvent &e) override;
    void mouseEnter(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;
	bool keyPressed(const KeyPress &key) override;
	void focusGained(FocusChangeType) override { DBG("Focus Gained"); }
	void focusLost(FocusChangeType) override { DBG("Focus Lost"); }

	Connector* getBaseConnector()
    {
        return sourceConnector != nullptr ? sourceConnector : destConnector;
    }

    //From Component Listener
    void componentMovedOrResized(Component& component, bool wasMoved, bool wasResize)override;


    bool isEditing()
    {
        return ((sourceConnector == nullptr && destConnector != nullptr) || (sourceConnector != nullptr && destConnector == nullptr));
    }


    bool setCandidateDropConnector(Connector * connector);
    void cancelCandidateDropConnector();

    bool finishEditing();


	// Inherited via Listener
	virtual void connectionDataLinkAdded(DataProcessorGraph::Connection * dataConnection) override;
	virtual void connectionDataLinkRemoved(DataProcessorGraph::Connection * dataConnection) override;
	virtual void connectionAudioLinkAdded(const std::pair<int,int> &audioConnection) override;
	virtual void connectionAudioLinkRemoved(const std::pair<int, int> & audioConnection) override;


    Component * getNodeManagerUI() { return (Component *)findParentComponentOfClass<NodeManagerUI>(); }

	InspectorEditor * getEditor() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeConnectionUI);


	
};


#endif  // NODECONNECTIONUI_H_INCLUDED
