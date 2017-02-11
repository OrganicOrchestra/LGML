/*
  ==============================================================================

    NodeConnectionUI.h
    Created: 4 Mar 2016 3:19:24pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODECONNECTIONUI_H_INCLUDED
#define NODECONNECTIONUI_H_INCLUDED


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

    NodeConnection* connection;


    Connector * sourceConnector;
    Connector * destConnector;
    Connector * candidateDropConnector;

    void setSourceConnector(Connector * c);
    void setDestConnector(Connector * c);


    Path path;
	Path hitPath;

    void paint(Graphics&)override;
    void resized()override;

	void buildPath();
	void buildHitPath(Array<Point<float>> points);

    void updateBoundsFromNodes();
    virtual bool hitTest(int x, int y) override{ return hitPath.contains((float)x, (float)y); }

    //interaction
    void mouseDown(const MouseEvent &e) override;
    void mouseEnter(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;
	bool keyPressed(const KeyPress &key) override;

	void componentParentHierarchyChanged(Component&) override { removeComponentListener(this); updateBoundsFromNodes(); }

	Connector* getBaseConnector()
    {
        return sourceConnector != nullptr ? sourceConnector : destConnector;
    }

	Connector* getSecondConnector()
	{
		return sourceConnector != nullptr ? destConnector : sourceConnector;
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

	InspectorEditor * createEditor() override;
  void handleCommandMessage(int cId)override;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeConnectionUI);



};


#endif  // NODECONNECTIONUI_H_INCLUDED
