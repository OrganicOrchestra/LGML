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
	NodeConnectionUI();
    ~NodeConnectionUI();

	typedef NodeBaseUI::ConnectorContainer::ConnectorComponent Connector;

	Connector * sourceConnector;
	Connector * destConnector;
	Connector * candidateDropConnector;

	void setSourceConnector(Connector * c);
	void setDestConnector(Connector * c);

	void paint(Graphics&);
	void resized();


	Connector* getBaseConnector()
	{
		return sourceConnector != nullptr ? sourceConnector : destConnector;
	}

	//Point<int> editingPos;
	//void setEditingPos(Point<int> pos);

	void componentMovedOrResized(Component& component,bool wasMoved,bool wasResize)
	{
		//DBG("MOVED detected from NodeConnectionUI");
		if (!isEditing())
		{
			Component * nmui = getNodeManagerUI();
			Point<int> cPos = ComponentUtil::getRelativeComponentPositionCenter(sourceConnector,nmui);
			Point<int> mPos = ComponentUtil::getRelativeComponentPositionCenter(destConnector,nmui);
			int minX = jmin<int>(cPos.x, mPos.x);
			int minY = jmin<int>(cPos.y, mPos.y);
			int tw = abs(cPos.x - mPos.x);
			int th = abs(cPos.y - mPos.y);
			int margin = 50;

			//DBG(cPos.toString() + " // " + mPos.toString());
			setBounds(minX - margin, minY - margin, tw + margin * 2, th + margin * 2);
		}
		

		repaint();
	}

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
