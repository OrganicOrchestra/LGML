/*
  ==============================================================================

    NodeConnectionUI.cpp
    Created: 4 Mar 2016 3:19:24pm
    Author:  bkupe

  ==============================================================================
*/


#include "NodeConnectionUI.h"
#include "NodeConnectionEditor.h"

//==============================================================================
NodeConnectionUI::NodeConnectionUI(NodeConnection * connection, Connector * sourceConnector, Connector * destConnector) :
	candidateDropConnector(nullptr),
	connection(connection),
	sourceConnector(nullptr), destConnector(nullptr)
{

	setSourceConnector(sourceConnector);
	setDestConnector(destConnector);

	if (connection != nullptr)
	{
		connection->addConnectionListener(this);
	}

	if (sourceConnector == nullptr || destConnector == nullptr)
	{
		setInterceptsMouseClicks(false, false);
	}

	addComponentListener(this);
	setWantsKeyboardFocus(true);
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

	if (connection != nullptr)
	{
		connection->removeConnectionListener(this);
	}
}

void NodeConnectionUI::paint (Graphics& g)
{

	//g.fillAll(Colours::purple.withAlpha(.2f));

    Point<float> sourcePos;
    Point<float> endPos;

    if (isEditing())
    {
        sourcePos = getLocalPoint(getBaseConnector(),getBaseConnector()->getLocalBounds().getCentre()).toFloat();
        endPos = (candidateDropConnector != nullptr)?
        getLocalPoint(candidateDropConnector, candidateDropConnector->getLocalBounds().getCentre()).toFloat():
        getMouseXYRelative().toFloat();
    }else
    {
        sourcePos = getLocalPoint(sourceConnector, sourceConnector->getLocalBounds().getCentre()).toFloat();
        endPos = getLocalPoint(destConnector, destConnector->getLocalBounds().getCentre()).toFloat();
    }

    Point<float> midPoint = (sourcePos + endPos) / 2;


    float smoothBigConnector = 1+ .01f*(jmax<float>(10,std::abs(endPos.x - sourcePos.x))-10);
    float anchorOffset = (endPos.x - sourcePos.x)/(2*smoothBigConnector);
    float sourceAnchorX = sourcePos.x + anchorOffset;
    float endAnchorX = endPos.x - anchorOffset;


    int hitMargin = 10;
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

	bool isAudio = getBaseConnector()->dataType == NodeConnection::ConnectionType::AUDIO;
    Colour baseColor = isAudio ? AUDIO_COLOR : DATA_COLOR;
	
	if (connection != nullptr)
	{
		if (isAudio && connection->audioConnections.size() == 0) baseColor = NORMAL_COLOR;
		if (!isAudio && connection->dataConnections.size() == 0) baseColor = NORMAL_COLOR;
	}

	if (isMouseOver()) baseColor = Colours::red;
	if (candidateDropConnector != nullptr) baseColor = Colours::yellow;
	if (isSelected) baseColor = HIGHLIGHT_COLOR;
    g.setColour(baseColor);
    g.strokePath(p, PathStrokeType(2.0f));

}

void NodeConnectionUI::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void NodeConnectionUI::updateBoundsFromNodes()
{
//  DBG("Update bounds from Nodes, is Editing ? " + String(isEditing()));
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

//      DBG(cPos.toString() + " // " + mPos.toString());

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
			selectThis();
            break;

        case 2:
            connection->remove();
            break;
        }
	}
	else
	{
		selectThis();

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

bool NodeConnectionUI::keyPressed(const KeyPress & key)
{
	if (!isSelected) return false;

	if (key.getKeyCode() == KeyPress::deleteKey || key.getKeyCode() == KeyPress::backspaceKey)
	{
		connection->remove();
		return true;
	}

	return false;
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

void NodeConnectionUI::componentMovedOrResized(Component &, bool, bool)
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

InspectorEditor * NodeConnectionUI::getEditor()
{
	return new NodeConnectionEditor(this);
}


void NodeConnectionUI::connectionDataLinkAdded(DataProcessorGraph::Connection *)
{
	repaint();
}

void NodeConnectionUI::connectionDataLinkRemoved(DataProcessorGraph::Connection *)
{
	repaint();
}

void NodeConnectionUI::connectionAudioLinkAdded(const std::pair<int, int> &)
{
	repaint();
}

void NodeConnectionUI::connectionAudioLinkRemoved(const std::pair<int, int> &)
{
	repaint();
}
