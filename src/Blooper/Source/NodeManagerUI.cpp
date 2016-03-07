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
	if (editingConnection != nullptr)
	{
		delete editingConnection;
		editingConnection = nullptr;
	}
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


void NodeManagerUI::connectionAdded(NodeConnection * connection)
{
	addConnectionUI(connection);
}

void NodeManagerUI::connectionEdited(NodeConnection * connection)
{
	//do nothing ?
}

void NodeManagerUI::connectionRemoved(NodeConnection * connection)
{
	removeConnectionUI(connection);
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

void NodeManagerUI::addConnectionUI(NodeConnection * connection)
{
	DBG("NMUI :: addConnectionUI From NodeManagerListener");

	if (getUIForConnection(connection) != nullptr)
	{
		DBG("AddConnectionUI :: already exists");
		return;
	}

	NodeBaseUI * n1 = getUIForNode(connection->sourceNode);
	NodeBaseUI * n2 = getUIForNode(connection->destNode);

	ConnectorComponent * c1 = (n1 != nullptr) ? n1->getFirstConnector(connection->connectionType,ConnectorComponent::OUTPUT): nullptr;
	ConnectorComponent * c2 = (n2 != nullptr) ? n2->getFirstConnector(connection->connectionType,ConnectorComponent::INPUT) : nullptr;


	NodeConnectionUI * cui = new NodeConnectionUI(connection, c1,c2);
	connectionsUI.add(cui);
	
	DBG("Add And MakeVisible connection");
	addAndMakeVisible(cui,0);
}

void NodeManagerUI::removeConnectionUI(NodeConnection * connection)
{
	NodeConnectionUI * nui = getUIForConnection(connection);
	if (nui == nullptr)
	{
		DBG("RemoveConnectionUI :: not exists");
		return;
	}

	connectionsUI.removeObject(nui);
	removeChildComponent(nui);
}

NodeConnectionUI * NodeManagerUI::getUIForConnection(NodeConnection* connection)
{
	for (int i = connectionsUI.size(); --i >= 0;)
	{
		NodeConnectionUI * cui = connectionsUI.getUnchecked(i);
		if (cui->connection == connection) return cui;
	}

	return nullptr;
}


void NodeManagerUI::createDataConnectionFromConnector(Connector * baseConnector, const String &dataName, const String &elementName)
{
	
	DBG("Create Data connection from connector : " + dataName + ", " + elementName);

	bool isOutputConnector = baseConnector->ioType == Connector::ConnectorIOType::OUTPUT;
	

	if (isOutputConnector)
	{
		editingConnection = new NodeConnectionUI(nullptr, baseConnector, nullptr);
	}else
	{
		editingConnection = new NodeConnectionUI(nullptr, nullptr, baseConnector);
	}

	addAndMakeVisible(editingConnection);

	if (dataName == "" && elementName == "")
	{
		baseConnector->removeMouseListener(this);
	}
}

void NodeManagerUI::createAudioConnectionFromConnector(Connector * baseConnector, uint32 channel)
{
	DBG("Create Audio Connection from connector " + String(channel));

	bool isOutputConnector = baseConnector->ioType == Connector::ConnectorIOType::OUTPUT;

	if (isOutputConnector)
	{
		editingConnection = new NodeConnectionUI(nullptr, baseConnector, nullptr);
	}
	else
	{
		editingConnection = new NodeConnectionUI(nullptr, nullptr, baseConnector);
	}

	addAndMakeVisible(editingConnection);
}

void NodeManagerUI::updateEditingConnection()
{
	if (editingConnection == nullptr) return;

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
		nodeManager->addConnection(editingConnection->sourceConnector->node, editingConnection->destConnector->node, c->dataType);
	}
	else
	{


	}
	//Delete the editing connection
	removeChildComponent(editingConnection);
	delete editingConnection;
	editingConnection = nullptr;
	
}

//Interaction Events
void NodeManagerUI::mouseDown(const MouseEvent & event)
{
	if (event.mods.isRightButtonDown())
	{
		ScopedPointer<PopupMenu> menu = new PopupMenu();
		ScopedPointer<PopupMenu> addNodeMenu = NodeFactory::getNodeTypesMenu(0);
		menu->addSubMenu("Add Node", *addNodeMenu);

		int result = menu->show();
		if (result >= 1 && result <= addNodeMenu->getNumItems())
		{
			nodeManager->addNode((NodeFactory::NodeType)(result-1));
		}
	}
	
}

void NodeManagerUI::mouseDrag(const MouseEvent & event)
{
	if (editingConnection != nullptr)
	{
		DBG("NMUI drag, target is editingConnection ?"+String(event.eventComponent == editingConnection->getBaseConnector()));
	}
}

void NodeManagerUI::mouseUp(const MouseEvent & event)
{

}