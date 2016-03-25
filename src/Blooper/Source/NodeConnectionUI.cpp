/*
  ==============================================================================

    NodeConnectionUI.cpp
    Created: 4 Mar 2016 3:19:24pm
    Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "NodeConnectionUI.h"

//==============================================================================
NodeConnectionUI::NodeConnectionUI(NodeConnection * connection, Connector * sourceConnector, Connector * destConnector) :
	ContourComponent(Colours::red),
	 candidateDropConnector(nullptr),
	connection(connection),
	sourceConnector(nullptr),destConnector(nullptr)
{
	
	setSourceConnector(sourceConnector);
	setDestConnector(destConnector);

	if (sourceConnector == nullptr || destConnector == nullptr)
	{
		setInterceptsMouseClicks(false, false);
	}

	addComponentListener(this);
	
}

NodeConnectionUI::~NodeConnectionUI()
{
	if (sourceConnector != nullptr)
	{
		sourceConnector->getNodeUI()->removeComponentListener(this);
	}

	sourceConnector = nullptr;

	if (destConnector != nullptr)
	{
		destConnector->getNodeUI()->removeComponentListener(this);
	}
	destConnector = nullptr;

	candidateDropConnector = nullptr;
}

void NodeConnectionUI::paint (Graphics& g)
{
	ContourComponent::paint(g);

	//DBG("PAINT !!");
	Point<int> sourcePos;
	Point<int> endPos;

	if (isEditing())
	{
        sourcePos = getLocalPoint(getBaseConnector(),getBaseConnector()->getLocalBounds().getCentre());
        endPos = (candidateDropConnector != nullptr)?
        getLocalPoint(candidateDropConnector, candidateDropConnector->getLocalBounds().getCentre()):
        getMouseXYRelative();
	}else
	{
		sourcePos = getLocalPoint(sourceConnector, sourceConnector->getLocalBounds().getCentre());
		endPos = getLocalPoint(destConnector, destConnector->getLocalBounds().getCentre());
	}

	Point<int> midPoint = (sourcePos + endPos) / 2;

	int minDist = -200;
	int maxDist = 100;
	float minOffset = 0;
	float maxOffset = 150;

	float anchorOffset = jmap<float>(endPos.x-sourcePos.x, maxDist, minDist, minOffset, maxOffset);
	anchorOffset = jmin<float>(jmax<float>(anchorOffset, minOffset), maxOffset);

	int sourceAnchorX = (sourcePos.x + midPoint.x)/2 + anchorOffset;
	int endAnchorX = (endPos.x+midPoint.x)/2  - anchorOffset;

	int hitMargin = 30;
	hitPath.clear();
	hitPath.startNewSubPath(sourcePos.x, sourcePos.y - hitMargin);
	hitPath.quadraticTo(sourceAnchorX, sourcePos.y - hitMargin, midPoint.x, midPoint.y - hitMargin);
	hitPath.quadraticTo(endAnchorX, endPos.y - hitMargin, endPos.x, endPos.y - hitMargin);
	hitPath.lineTo(endPos.x, endPos.y + hitMargin);
	hitPath.quadraticTo(endAnchorX, endPos.y + hitMargin, midPoint.x, midPoint.y + hitMargin);
	hitPath.quadraticTo(sourceAnchorX, sourcePos.y + hitMargin, sourcePos.x, sourcePos.y + hitMargin);
	hitPath.closeSubPath();

	Path p;
	p.startNewSubPath(sourcePos.x, sourcePos.y);        
	p.quadraticTo(sourceAnchorX, sourcePos.y, midPoint.x, midPoint.y);
	p.quadraticTo(endAnchorX, endPos.y, endPos.x, endPos.y);

	Colour baseColor = getBaseConnector()->dataType == NodeConnection::ConnectionType::AUDIO ? AUDIO_COLOR : DATA_COLOR;
	g.setColour((candidateDropConnector != nullptr) ? Colours::yellow : isMouseOver()?Colours::orange:baseColor );
	g.strokePath(p, PathStrokeType(2.0f));

}

void NodeConnectionUI::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void NodeConnectionUI::updateBoundsFromNodes()
{
	DBG("Update bounds from Nodes, is Editing ? " + String(isEditing()));
	if (!isEditing())
	{
		Component * nmui = getNodeManagerUI();
		Point<int> cPos = nmui->getLocalPoint(sourceConnector, sourceConnector->getLocalBounds().getCentre());
		Point<int> mPos = nmui->getLocalPoint(destConnector, destConnector->getLocalBounds().getCentre());

		int minX = jmin<int>(cPos.x, mPos.x);
		int minY = jmin<int>(cPos.y, mPos.y);
		int tw = abs(cPos.x - mPos.x);
		int th = abs(cPos.y - mPos.y);
		int margin = 50;

		DBG(cPos.toString() + " // " + mPos.toString());

		setBounds(minX - margin, minY - margin, tw + margin * 2, th + margin * 2);

		repaint();
	}

	

}

void NodeConnectionUI::mouseDown(const MouseEvent & e)
{
	if (e.mods.isRightButtonDown())
	{
		PopupMenu m;
		m.addItem(1,"Edit connections...");
		m.addItem(2, "Delete all connections");
		
		int result = m.show();
		switch (result)
		{
		case 1:
			//edit connection
			break;

		case 2:
			connection->remove();
			break;
		}
	}
}

void NodeConnectionUI::mouseEnter(const MouseEvent &)
{
	repaint();
}

void NodeConnectionUI::mouseExit(const MouseEvent &)
{
	repaint();
}

void NodeConnectionUI::setSourceConnector(Connector * c)
{
	if (sourceConnector != nullptr)
	{
		sourceConnector->getNodeUI()->removeComponentListener(this);
	}
	sourceConnector = c;

	if (sourceConnector != nullptr)
	{
		sourceConnector->getNodeUI()->addComponentListener(this);
	}

	repaint();
}

void NodeConnectionUI::setDestConnector(Connector * c)
{
	if (destConnector != nullptr)
	{
		destConnector->getNodeUI()->removeComponentListener(this);
	}

	destConnector = c;

	if (destConnector != nullptr)
	{
		destConnector->getNodeUI()->addComponentListener(this);
	}

	repaint();
}

void NodeConnectionUI::componentMovedOrResized(Component & component, bool wasMoved, bool wasResize)
{
	updateBoundsFromNodes();
}

bool NodeConnectionUI::setCandidateDropConnector(Connector * connector)
{
	//check if connector can accept data
	if (getBaseConnector() == connector) return false;
	if (getBaseConnector()->ioType == connector->ioType) return false;
	if (getBaseConnector()->dataType != connector->dataType) return false;

	candidateDropConnector = connector;

	return true;
}

void NodeConnectionUI::cancelCandidateDropConnector()
{
	candidateDropConnector = nullptr;
}

bool NodeConnectionUI::finishEditing()
{
	bool success = candidateDropConnector != nullptr;
	if(success)
	{
		if (sourceConnector == nullptr)
		{
			setSourceConnector(candidateDropConnector);
		}
		else
		{
			setDestConnector(candidateDropConnector);

		}
	}

	candidateDropConnector = nullptr;

	

	return success;
}

/*
void NodeConnectionUI::setEditingPos(Point<int> pos)
{
	editingPos = pos;
	repaint();
}
*/