/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include "NodeFactory.h"
#include "../Impl/DummyNode.h"
#include "../Impl/AudioMixerNode.h"
#include "../../Data/DataInNode.h"
#include "../Impl/Spat2DNode.h"
#include "../Impl/VSTNode.h"
#include "../Impl/AudioDeviceInNode.h"
#include "../Impl/AudioDeviceOutNode.h"
#include "../Impl/LooperNode.h"
#include "../Impl/JsNode.h"

#include "../NodeContainer/NodeContainer.h"

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

        case NodeType::Spat2DType:
            n = new Spat2DNode();
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

        case NodeType::JsNodeType:
            n= new JsNode();
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
	int numElements = numElementsInArray(nodeTypeNames) - userCanAddStartType +1;

    for (int i = 0; i < numElements; i++)
    {
        p->addItem(i+menuIdOffset, nodeTypeNames[i+userCanAddStartType-1]);
    }

    return p;
}

NodeType NodeFactory::getTypeFromString(const String & s) {

	//DBG("Get type from string " << s);

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
