/*
  ==============================================================================

    NodeConnectionUI.h
    Created: 4 Mar 2016 3:19:24pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODECONNECTIONUI_H_INCLUDED
#define NODECONNECTIONUI_H_INCLUDED

#include "JuceHeader.h"
#include "NodeBaseUI.h"
#include "NodeManagerUI.h"

//==============================================================================
/*
*/
class NodeConnectionUI : public ContourComponent, public ComponentListener
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

	void paint(Graphics&);
	void resized();
	void componentParentHierarchyChanged(Component&) override { removeComponentListener(this); updateBoundsFromNodes(); }

	void updateBoundsFromNodes();
	virtual bool hitTest(int x, int y) { return hitPath.contains(x, y); }

	//interaction
	void mouseDown(const MouseEvent &e) override;
	void mouseEnter(const MouseEvent &e) override;
	void mouseExit(const MouseEvent &e) override;

	Connector* getBaseConnector()
	{
		return sourceConnector != nullptr ? sourceConnector : destConnector;
	}

	//From Component Listener
	void componentMovedOrResized(Component& component, bool wasMoved, bool wasResize);
	

	bool isEditing()
	{
		return ((sourceConnector == nullptr && destConnector != nullptr) || (sourceConnector != nullptr && destConnector == nullptr));
	}


	bool setCandidateDropConnector(Connector * connector);
	void cancelCandidateDropConnector();

	bool finishEditing();

	Component * getNodeManagerUI() { return (Component *)findParentComponentOfClass<NodeManagerUI>(); }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeConnectionUI)
};


#endif  // NODECONNECTIONUI_H_INCLUDED
