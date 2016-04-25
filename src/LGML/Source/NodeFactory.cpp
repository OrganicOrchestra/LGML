/*
  ==============================================================================

    NodeFactory.cpp
    Created: 25 Apr 2016 5:21:10pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeFactory.h"


NodeBase * NodeFactory::createNode(NodeManager * nodeManager, NodeType nodeType, uint32 nodeId)
{
	NodeBase * n = nullptr;

	switch (nodeType)
	{
	case NodeType::Dummy:
		n = new DummyNode(nodeManager, nodeId);
		break;

	case NodeType::AudioMixer:
		n = new AudioMixerNode(nodeManager, nodeId);
		break;

	case NodeType::DataIn:
		n = new DataInNode(nodeManager, nodeId);
		break;

	case NodeType::Spat:
		n = new SpatNode(nodeManager, nodeId);
		break;
	case NodeType::Looper:
		n = new LooperNode(nodeManager, nodeId);
		break;
	case NodeType::VST:
		n = new VSTNode(nodeManager, nodeId);
		break;

	case NodeType::AudioIn:
		n = new AudioInNode(nodeManager, nodeId);
		break;

	case NodeType::AudioOut:
		n = new AudioOutNode(nodeManager, nodeId);
		break;
	case NodeType::UNKNOWN:
		DBG("NodeFactory : not found type for node");
		return nullptr;
	default:
		jassert(false);
		break;
	}
	n->nodeType = nodeType;

	return n;
}

PopupMenu * NodeFactory::getNodeTypesMenu(int menuIdOffset)
{
	PopupMenu * p = new PopupMenu();
	for (int i = 0; i < numElementsInArray(nodeTypeNames); i++)
	{
		p->addItem(menuIdOffset + i + 1, nodeTypeNames[i]);
	}

	return p;
}

NodeFactory::NodeType NodeFactory::getTypeFromString(const String & s) {
	for (int i = 0; i < numElementsInArray(nodeTypeNames); i++)
	{
		if (s == nodeTypeNames[i]) { return NodeType(i); }
	}
	return UNKNOWN;
}

String NodeFactory::nodeTypeToString(NodeType t) {
	if (t<0 || t > UNKNOWN)return String::empty;
	return nodeTypeNames[(int)t];
}

String NodeFactory::nodeToString(NodeBase * n) {
	NodeType t = (NodeType)n->nodeType;
	if (t<0 || t > UNKNOWN)return String::empty;
	return nodeTypeNames[(int)t];
}
