#include "NodeManagerUI.h"
#include "ConnectorComponent.h"

ConnectorComponent::ConnectorComponent(ConnectorIOType ioType, NodeConnection::ConnectionType dataType, NodeBase * node) :
	ioType(ioType), dataType(dataType), node(node)
{
	boxColor = dataType == NodeConnection::ConnectionType::AUDIO ? AUDIO_COLOR : DATA_COLOR;
	setSize(10,10);
}

void ConnectorComponent::mouseDown(const MouseEvent & e)
{
	NodeManagerUI * nmui = getNodeManagerUI();
	nmui->createConnectionFromConnector(this);
}

void ConnectorComponent::mouseDrag(const MouseEvent & e)
{
	getNodeManagerUI()->updateEditingConnection();
}

void ConnectorComponent::mouseUp(const MouseEvent & e)
{
	getNodeManagerUI()->finishEditingConnection(this);
}
NodeManagerUI * ConnectorComponent::getNodeManagerUI() const noexcept
{
	return findParentComponentOfClass<NodeManagerUI>();
}

NodeBaseUI * ConnectorComponent::getNodeUI() const noexcept
{
	return findParentComponentOfClass<NodeBaseUI>();
}
