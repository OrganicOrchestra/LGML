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
NodeConnectionUI::NodeConnectionUI() :ContourComponent(Colours::red),sourceConnector(nullptr),destConnector(nullptr), candidateDropConnector(nullptr)
{
	setInterceptsMouseClicks(false, false);
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
		sourcePos = ComponentUtil::getRelativeComponentPositionCenter(getBaseConnector(), this);
		endPos = (candidateDropConnector != nullptr)?ComponentUtil::getRelativeComponentPositionCenter(candidateDropConnector, this):getMouseXYRelative();
	}else
	{
		sourcePos = ComponentUtil::getRelativeComponentPositionCenter(sourceConnector, this);
		endPos = ComponentUtil::getRelativeComponentPositionCenter(destConnector, this);
	}
	
	//if (candidateDropConnector != nullptr) DBG("HAS CANDIDATE");

	//DBG("edit connection : " + sourcePos.toString() +"/" + endPos.toString());
	
	g.setColour((candidateDropConnector != nullptr) ? Colours::yellow:(getBaseConnector()->dataType == Connector::ConnectorDataType::AUDIO ? AUDIO_COLOR : DATA_COLOR));
	
	int midX = (sourcePos.x + endPos.x) / 2;
	
	Path p;
	p.startNewSubPath(sourcePos.x,sourcePos.y);          // move the current position to (10, 10)
	p.cubicTo(midX,sourcePos.y,midX,endPos.y,endPos.x,endPos.y); // draw a curve that ends at (5, 50)
	//p.closeSubPath();                          // close the subpath with a line back to (10, 10)
	
	
	g.strokePath(p, PathStrokeType(2.0f));
	//g.drawLine(sourcePos.x, sourcePos.y, endPos.x, endPos.y, 3);

	//g.drawRect(endPos.x-5, endPos.y-5, 10, 10);
}

void NodeConnectionUI::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void NodeConnectionUI::setSourceConnector(Connector * c)
{
	sourceConnector = c;
	repaint();
}

void NodeConnectionUI::setDestConnector(Connector * c)
{
	destConnector = c;
	repaint();
}

bool NodeConnectionUI::setCandidateDropConnector(Connector * connector)
{
	//check if connector can accept data
	if (getBaseConnector() == connector) return true;
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

		sourceConnector->getNodeUI()->addComponentListener(this);
		destConnector->getNodeUI()->addComponentListener(this);
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