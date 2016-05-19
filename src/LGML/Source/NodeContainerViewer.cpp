/*
  ==============================================================================

    NodeContainerViewer.cpp
    Created: 18 May 2016 8:22:16pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeContainerViewer.h"
#include "NodeConnectionUI.h"
#include "NodeConnectionEditor.h"
#include "Inspector.h"

NodeContainerViewer::NodeContainerViewer(NodeContainer * container) :
	nodeContainer(container),
	editingConnection(nullptr)
{
	setInterceptsMouseClicks(true, true);

	nodeContainer->addNodeContainerListener(this);
	DBG("Node Container Viewer : nodes In container = " << nodeContainer->nodes.size());
	for (auto &n : nodeContainer->nodes)
	{
		addNodeUI((NodeBase*)n);

	}
	for (auto &c : nodeContainer->connections)
	{
		addConnectionUI(c);
	}
} 

NodeContainerViewer::~NodeContainerViewer()
{
	nodeContainer->removeNodeContainerListener(this);
	clear();
}

void NodeContainerViewer::clear()
{
	if (editingConnection != nullptr)
	{
		editingConnection->getBaseConnector()->removeMouseListener(this);
		delete editingConnection;
		editingConnection = nullptr;
	}

	while (connectionsUI.size() > 0)
	{
		removeConnectionUI(connectionsUI[0]->connection);
	}

	while (nodesUI.size() > 0)
	{
		removeNodeUI(nodesUI[0]->node);
	}
}

void NodeContainerViewer::resized()
{
}

void NodeContainerViewer::nodeAdded(ConnectableNode * node)
{
	addNodeUI((NodeBase*)node);
}

void NodeContainerViewer::nodeRemoved(ConnectableNode * node)
{
	removeNodeUI((NodeBase*)node);
}


void NodeContainerViewer::connectionAdded(NodeConnection * connection)
{
	addConnectionUI(connection);
}

void NodeContainerViewer::connectionRemoved(NodeConnection * connection)
{
	removeConnectionUI(connection);
}

void NodeContainerViewer::addNodeUI(NodeBase * node)
{

	if (getUIForNode(node) == nullptr)
	{
		NodeBaseUI * nui = node->createUI();
		nodesUI.add(nui);
		addAndMakeVisible(nui);
		nui->setTopLeftPosition(node->xPosition->intValue(), node->yPosition->intValue());
	}
	else
	{
		//ui for this node already in list
	}
}



void NodeContainerViewer::removeNodeUI(NodeBase * node)
{
	//DBG("Remove NodeUI");
	NodeBaseUI * nui = getUIForNode(node);
	if (nui != nullptr)
	{
		nodesUI.removeObject(nui);
		removeChildComponent(nui);
	}
	else
	{
		//nodeBaseUI isn't in list
	}
}


NodeBaseUI * NodeContainerViewer::getUIForNode(NodeBase * node)
{
	for (int i = nodesUI.size(); --i >= 0;)
	{
		NodeBaseUI * nui = nodesUI.getUnchecked(i);
		if (nui->node == node) return nui;
	}

	return nullptr;
}

void NodeContainerViewer::addConnectionUI(NodeConnection * connection)
{

	if (getUIForConnection(connection) != nullptr)
	{
		DBG("AddConnectionUI :: already exists");
		return;
	}

	NodeBaseUI * n1 = getUIForNode((NodeBase*)connection->sourceNode);
	NodeBaseUI * n2 = getUIForNode((NodeBase*)connection->destNode);

	ConnectorComponent * c1 = (n1 != nullptr) ? n1->getFirstConnector(connection->connectionType, ConnectorComponent::OUTPUT) : nullptr;
	ConnectorComponent * c2 = (n2 != nullptr) ? n2->getFirstConnector(connection->connectionType, ConnectorComponent::INPUT) : nullptr;


	NodeConnectionUI * cui = new NodeConnectionUI(connection, c1, c2);
	connectionsUI.add(cui);

	addAndMakeVisible(cui, 0);
}

void NodeContainerViewer::removeConnectionUI(NodeConnection * connection)
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

NodeConnectionUI * NodeContainerViewer::getUIForConnection(NodeConnection* connection)
{
	for (int i = connectionsUI.size(); --i >= 0;)
	{
		NodeConnectionUI * cui = connectionsUI.getUnchecked(i);
		if (cui->connection == connection) return cui;
	}

	return nullptr;
}


void NodeContainerViewer::createDataConnectionFromConnector(Connector * baseConnector)
{

	//DBG("Create Data connection from connector");

	if (editingConnection != nullptr)
	{
		DBG("Already editing a connection !");
		return;
	}

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

	baseConnector->addMouseListener(this, false);
}

void NodeContainerViewer::createAudioConnectionFromConnector(Connector * baseConnector)
{
	if (editingConnection != nullptr)
	{
		DBG("Already editing a connection !");
		return;
	}

	bool isOutputConnector = baseConnector->ioType == Connector::ConnectorIOType::OUTPUT;

	if (isOutputConnector)
	{
		editingConnection = new NodeConnectionUI(nullptr, baseConnector, nullptr);
	}
	else
	{
		editingConnection = new NodeConnectionUI(nullptr, nullptr, baseConnector);
	}

	// editingChannel = (uint32)-1; //temp, will be able to select which channel later

	baseConnector->addMouseListener(this, false);

	addAndMakeVisible(editingConnection);
}

void NodeContainerViewer::updateEditingConnection()
{
	if (editingConnection == nullptr) return;

	Point<int> cPos = getLocalPoint(editingConnection->getBaseConnector(), editingConnection->getBaseConnector()->getLocalBounds().getCentre());
	Point<int> mPos = getMouseXYRelative();
	int minX = jmin<int>(cPos.x, mPos.x);
	int minY = jmin<int>(cPos.y, mPos.y);
	int tw = abs(cPos.x - mPos.x);
	int th = abs(cPos.y - mPos.y);
	int margin = 50;

	checkDropCandidates();

	editingConnection->setBounds(minX - margin, minY - margin, tw + margin * 2, th + margin * 2);
}

bool NodeContainerViewer::checkDropCandidates()
{
	Connector * candidate = nullptr;
	for (int i = 0; i < nodesUI.size(); i++)
	{
		Array<Connector *> compConnectors = nodesUI.getUnchecked(i)->getComplementaryConnectors(editingConnection->getBaseConnector());

		for (int j = 0; j < compConnectors.size(); j++)
		{
			Connector * c = compConnectors.getUnchecked(j);
			float dist = (float)(c->getMouseXYRelative().getDistanceFromOrigin());
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

bool NodeContainerViewer::setCandidateDropConnector(Connector * connector)
{
	if (!isEditingConnection()) return false;

	bool result = editingConnection->setCandidateDropConnector(connector);
	editingConnection->candidateDropConnector->addMouseListener(this, false);
	return result;
}

void NodeContainerViewer::cancelCandidateDropConnector()
{
	if (!isEditingConnection()) return;
	if (editingConnection->candidateDropConnector != nullptr) editingConnection->candidateDropConnector->removeMouseListener(this);
	editingConnection->cancelCandidateDropConnector();


}

void NodeContainerViewer::finishEditingConnection()
{
	//DBG("Finish Editing connection");
	if (!isEditingConnection()) return;

	//bool isEditingDataOutput = editingConnection->getBaseConnector()->ioType == Connector::ConnectorIOType::OUTPUT;
	editingConnection->getBaseConnector()->removeMouseListener(this);
	if (editingConnection->candidateDropConnector != nullptr) editingConnection->candidateDropConnector->removeMouseListener(this);

	bool isDataConnection = editingConnection->getBaseConnector()->dataType == NodeConnection::ConnectionType::DATA;

	if (isDataConnection) //DATA
	{
		bool success = editingConnection->finishEditing();
		if (success)
		{
			nodeContainer->addConnection(editingConnection->sourceConnector->node, editingConnection->destConnector->node, editingConnection->getBaseConnector()->dataType);
		}
	}
	else //AUDIO
	{
		bool success = editingConnection->finishEditing();

		if (success)
		{
			nodeContainer->addConnection(editingConnection->sourceConnector->node, editingConnection->destConnector->node, editingConnection->getBaseConnector()->dataType);
		}
	}

	removeChildComponent(editingConnection);
	delete editingConnection;
	editingConnection = nullptr;

}


//Interaction Events
void NodeContainerViewer::mouseDown(const MouseEvent & event)
{
	if (event.eventComponent == this)
	{
		if (event.mods.isRightButtonDown())
		{

			int menuOffset = 1;
			PopupMenu   menu;//(new PopupMenu());
			ScopedPointer<PopupMenu> addNodeMenu(NodeFactory::getNodeTypesMenu(menuOffset));
			menu.addSubMenu("Add Node", *addNodeMenu);

			int result = menu.show();

			if (result > 0 && result < addNodeMenu->getNumItems() + menuOffset)
			{
				NodeBase * n = (NodeBase*)nodeContainer->addNode((NodeType)(result- menuOffset));
				Point<int> mousePos = getMouseXYRelative();
				n->xPosition->setValue((float)mousePos.x);
				n->yPosition->setValue((float)mousePos.y);
			}
		}
	}

}

void NodeContainerViewer::mouseMove(const MouseEvent &)
{
	if (editingConnection != nullptr)
	{
		//DBG("NMUI mouse mouve while editing connection");
		updateEditingConnection();
	}
}

void NodeContainerViewer::mouseDrag(const MouseEvent & event)
{
	if (editingConnection != nullptr)
	{
		if (event.eventComponent == editingConnection->getBaseConnector())
		{
			updateEditingConnection();
		}
	}
}

void NodeContainerViewer::mouseUp(const MouseEvent &)
{
	if (isEditingConnection())
	{
		finishEditingConnection();
	}
	else
	{
		if (Inspector::getInstanceWithoutCreating() != nullptr) Inspector::getInstance()->setCurrentComponent(nullptr);
	}

}


void NodeContainerViewer::childBoundsChanged(Component *) {
	resizeToFitNodes();
}


void NodeContainerViewer::resizeToFitNodes() {

	Rectangle<int> _bounds(getLocalBounds());

	for (auto &n : nodesUI) {

		Rectangle<int> r = n->getBoundsInParent();
		_bounds = _bounds.getUnion(r);

	}

	setSize(_bounds.getWidth(), _bounds.getHeight());

}
