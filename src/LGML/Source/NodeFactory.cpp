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
        case NodeType::DummyType:
            n = new DummyNode(nodeManager, nodeId);
            break;

        case NodeType::AudioMixerType:
            n = new AudioMixerNode(nodeManager, nodeId);
            break;

        case NodeType::DataInType:
            n = new DataInNode(nodeManager, nodeId);
            break;

        case NodeType::SpatType:
            n = new SpatNode(nodeManager, nodeId);
            break;
        case NodeType::LooperType:
            n = new LooperNode(nodeManager, nodeId);
            break;
        case NodeType::VSTType:
            n = new VSTNode(nodeManager, nodeId);
            break;

        case NodeType::AudioInType:
            n = new AudioInNode(nodeManager, nodeId);
            break;

        case NodeType::AudioOutType:
            n = new AudioOutNode(nodeManager, nodeId);
            break;
            
        case NodeType::UNKNOWN_TYPE:
            DBG("NodeFactory : not found type for node");
            return nullptr;
        default:
            jassert(false);
            break;
    }
    n->nodeTypeUID = nodeType;

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

NodeType NodeFactory::getTypeFromString(const String & s) {
    for (int i = 0; i < numElementsInArray(nodeTypeNames); i++)
    {
        if (s == nodeTypeNames[i]) { return NodeType(i+1); }
    }
    return UNKNOWN_TYPE;
}

String NodeFactory::nodeTypeToString(NodeType t) {
    if (t<=UNKNOWN_TYPE || t >=  numElementsInArray(nodeTypeNames))return String::empty;
    return nodeTypeNames[(int)t];
}

String NodeFactory::nodeToString(NodeBase * n) {
    return nodeTypeToString((NodeType)n->nodeTypeUID);
}
