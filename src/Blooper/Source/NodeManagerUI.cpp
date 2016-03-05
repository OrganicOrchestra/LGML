/*
  ==============================================================================

    NodeManagerUI.cpp
    Created: 3 Mar 2016 10:38:22pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeManagerUI.h"
#include "NodeConnectionUI.h"

//==============================================================================
NodeManagerUI::NodeManagerUI(NodeManager * nodeManager) :nodeManager(nodeManager),editingConnection(nullptr)
{
	nodeManager->addListener(this);
	setInterceptsMouseClicks(true, true);
}

NodeManagerUI::~NodeManagerUI()
{

}

/*
void NodeManagerUI::setNodeManager(NodeManager * nodeManager)
{
	this->nodeManager = nodeManager;
	nodeManager->addListener(this);
}
*/

void NodeManagerUI::clear()
{
	nodesUI.clear();
}

void NodeManagerUI::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (BG_COLOR);   // clear the background
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void NodeManagerUI::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void NodeManagerUI::nodeAdded(NodeBase * node)
{
	addNodeUI(node);
}

void NodeManagerUI::nodeRemoved(NodeBase * node)
{
	removeNodeUI(node);
}


void NodeManagerUI::audioConnectionAdded(AudioProcessorGraph::Connection * connection)
{
	//addAudioConnectionUI(connection);
}

void NodeManagerUI::audioConnectionRemoved(AudioProcessorGraph::Connection * connection)
{
	//removeAudioConnectionUI(connection);
}

void NodeManagerUI::dataConnectionAdded(DataProcessorGraph::Connection * connection)
{
	//addDataConnectionUI(connection);
}

void NodeManagerUI::dataConnectionRemoved(DataProcessorGraph::Connection * connection)
{
	//removeDataConnectionUI(connection);
}


void NodeManagerUI::addNodeUI(NodeBase * node)
{
	
	DBG("Add Node UI");
	if (getUIForNode(node) == nullptr)
	{
		NodeBaseUI * nui = node->createUI();
		nodesUI.add(nui);
		addAndMakeVisible(nui);
		Point<int> mousePos = getMouseXYRelative();
		nui->setCentrePosition(mousePos.x, mousePos.y);
	}
	else
	{
		//ui for this node already in list
	}
}

void NodeManagerUI::removeNodeUI(NodeBase * node)
{
	DBG("Remove NodeUI");
	NodeBaseUI * nui = getUIForNode(node);
	if (nui != nullptr)
	{
		nodesUI.remove(nodesUI.indexOf(nui));
		removeChildComponent(nui);
	}
	else
	{
		//nodeBaseUI isn't in list
	}
}


NodeBaseUI * NodeManagerUI::getUIForNode(NodeBase * node)
{
	for (int i = nodesUI.size(); --i >= 0;)
	{
		NodeBaseUI * nui = nodesUI.getUnchecked(i);
		if (nui->node == node) return nui;
	}

	return nullptr;
}

void NodeManagerUI::mouseDown(const MouseEvent & event)
{
	if (event.mods.getCurrentModifiers().isCtrlDown())
	{
		nodeManager->addNode(NodeFactory::NodeType::Dummy);
	}
}



void NodeManagerUI::createConnectionFromConnector(Connector * baseConnector)
{
	
	Point<int> globalConnectorPos = ComponentUtil::getRelativeComponentPosition(baseConnector, this);
	
	bool isOutputConnector = baseConnector->ioType == Connector::ConnectorIOType::OUTPUT;
	editingConnection = new NodeConnectionUI();
	if (isOutputConnector)
	{
		editingConnection->setSourceConnector(baseConnector);
	}
	else
	{
		editingConnection->setDestConnector(baseConnector);
	}

	addAndMakeVisible(editingConnection);
	
}

void NodeManagerUI::updateEditingConnection()
{

	if (editingConnection == nullptr) return;

	DBG("update editing connection");

	Point<int> cPos = ComponentUtil::getRelativeComponentPosition(editingConnection->getBaseConnector(), this);
	Point<int> mPos = getMouseXYRelative();
	int minX = jmin<int>(cPos.x, mPos.x);
	int minY = jmin<int>(cPos.y, mPos.y);
	int tw = abs(cPos.x - mPos.x);
	int th = abs(cPos.y - mPos.y);
	int margin = 50;

	checkDropCandidates();

	editingConnection->setBounds(minX-margin, minY-margin, tw+margin*2, th+margin*2);
}

bool NodeManagerUI::checkDropCandidates()
{
	Connector * candidate = nullptr;
	for (int i = 0; i < nodesUI.size(); i++)
	{
		Array<Connector *> compConnectors = nodesUI.getUnchecked(i)->getComplementaryConnectors(editingConnection->getBaseConnector());

		for (int j = 0; j < compConnectors.size(); j++)
		{
			Connector * c = compConnectors.getUnchecked(j);
			float dist = c->getMouseXYRelative().getDistanceFromOrigin();
			if (dist < 20)
			{
				candidate = c;
				setCandidateDropConnector(c);
				return true;
			}
		}
	}

	cancelCandidateDropConnector();
	return false;
}

bool NodeManagerUI::setCandidateDropConnector(Connector * connector)
{
	if (!isEditingConnection()) return false;
	
	return editingConnection->setCandidateDropConnector(connector);
	return true;
}

void NodeManagerUI::cancelCandidateDropConnector()
{
	if (!isEditingConnection()) return;
	editingConnection->cancelCandidateDropConnector();
}

void NodeManagerUI::finishEditingConnection(Connector * c)
{
	if (!isEditingConnection()) return;
	bool success = editingConnection->finishEditing();
	editingConnection->toBack();

	DBG("Finish editing, sucess ?" + String(success));
	if (success)
	{
		connectionsUI.add(editingConnection);
	}else
	{
		removeChildComponent(editingConnection);
		delete editingConnection;
		
	}

	editingConnection = nullptr;
	
}

void NodeManagerUI::addAudioConnectionUI(AudioProcessorGraph::Connection * connection)
{
}

void NodeManagerUI::removeAudioConnectionUI(AudioProcessorGraph::Connection * connection)
{
}


NodeConnectionUI * NodeManagerUI::getUIForAudioConnection(AudioProcessorGraph::Connection * connection)
{
	return nullptr;
}

void NodeManagerUI::addDataConnectionUI(DataProcessorGraph::Connection * connection)
{
}

void NodeManagerUI::removeDataConnectionUI(DataProcessorGraph::Connection * connection)
{
}


NodeConnectionUI * NodeManagerUI::getUIForDataConnection(DataProcessorGraph::Connection * connection)
{
	return nullptr;
}
