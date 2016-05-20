/*
 ==============================================================================

 NodeFactory.cpp
 Created: 25 Apr 2016 5:21:10pm
 Author:  bkupe

 ==============================================================================
 */

#include "NodeFactory.h"
#include "DummyNode.h"
#include "AudioMixerNode.h"
#include "DataInNode.h"
#include "SpatNode.h"
#include "VSTNode.h"
#include "AudioDeviceInNode.h"
#include "AudioDeviceOutNode.h"
#include "LooperNode.h"

#include "NodeContainer.h"

NodeFactory::NodeFactory()
{
}

NodeFactory::~NodeFactory()
{
}

ConnectableNode * NodeFactory::createNode(NodeType nodeType)
{
    ConnectableNode * n = nullptr;

    switch (nodeType)
    {
	case NodeType::UNKNOWN_TYPE:
		DBG("NodeFactory : not found type for node");
		break;

		case NodeType::ContainerType:
			n = new NodeContainer();
			break;

		case NodeType::ContainerInType:
			n = new ContainerInNode();
				break;

		case NodeType::ContainerOutType:
			n = new ContainerOutNode();
			break;

        case NodeType::DummyType:
            n = new DummyNode();
            break;

        case NodeType::AudioMixerType:
            n = new AudioMixerNode();
            break;

        case NodeType::DataInType:
            n = new DataInNode();
            break;

        case NodeType::SpatType:
            n = new SpatNode();
            break;
        case NodeType::LooperType:
            n = new LooperNode();
            break;
        case NodeType::VSTType:
            n = new VSTNode();
            break;

        case NodeType::AudioDeviceInType:
            n = new AudioDeviceInNode();
            break;

        case NodeType::AudioDeviceOutType:
            n = new AudioDeviceOutNode();
            break;

        
        default:
            break;
    }

	jassert(n != nullptr);
    return n;
}

PopupMenu * NodeFactory::getNodeTypesMenu(int menuIdOffset)
{
    PopupMenu * p = new PopupMenu();
	int numElements = numElementsInArray(nodeTypeNames) - userCanAddStartType;

    for (int i = 0; i < numElements; i++)
    {
        p->addItem(i+menuIdOffset, nodeTypeNames[i+userCanAddStartType-1]);
    } 

    return p;
}

NodeType NodeFactory::getTypeFromString(const String & s) {

	DBG("Get type from string " << s);

    for (int i = 0; i < numElementsInArray(nodeTypeNames); i++)
    {
        if (s == nodeTypeNames[i]) { return NodeType(i+1); }
    }
    return UNKNOWN_TYPE;
}

NodeType NodeFactory::getTypeForIndex(int nodeTypeIndex, bool includeUserOffset)
{
	int targetIndex = nodeTypeIndex + (includeUserOffset ? userCanAddStartType : 0);
	return (NodeType)(targetIndex);
}

String NodeFactory::nodeTypeToString(NodeType t) {
    if (t<=UNKNOWN_TYPE || t-1 >  numElementsInArray(nodeTypeNames))return String::empty;
    return nodeTypeNames[(int)t-1];
}

String NodeFactory::nodeToString(ConnectableNode * n) {
    return nodeTypeToString(n->type);
}
