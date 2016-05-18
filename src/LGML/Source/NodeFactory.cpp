/*
 ==============================================================================

 NodeFactory.cpp
 Created: 25 Apr 2016 5:21:10pm
 Author:  bkupe

 ==============================================================================
 */

#include "NodeFactory.h"


NodeBase * NodeFactory::createNode(NodeType nodeType, uint32 nodeId)
{
    NodeBase * n = nullptr;

    switch (nodeType)
    {
        case NodeType::DummyType:
            n = new DummyNode(nodeId);
            break;

        case NodeType::AudioMixerType:
            n = new AudioMixerNode(nodeId);
            break;

        case NodeType::DataInType:
            n = new DataInNode(nodeId);
            break;

        case NodeType::SpatType:
            n = new SpatNode(nodeId);
            break;
        case NodeType::LooperType:
            n = new LooperNode(nodeId);
            break;
        case NodeType::VSTType:
            n = new VSTNode(nodeId);
            break;

        case NodeType::AudioInType:
            n = new AudioInNode(nodeId);
            break;

        case NodeType::AudioOutType:
            n = new AudioOutNode(nodeId);
            break;

        case NodeType::UNKNOWN_TYPE:
            DBG("NodeFactory : not found type for node");
            return nullptr;
        default:
            jassert(false);
            break;
    }
    n->nodeTypeUID = (int)nodeType;

    return n;
}

PopupMenu * NodeFactory::getNodeTypesMenu(int menuIdOffset)
{
    PopupMenu * p = new PopupMenu();
    for (int i = 0; i < numElementsInArray(nodeTypeNames); i++)
    {
        p->addItem(menuIdOffset + i, nodeTypeNames[i]);
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
    if (t<=UNKNOWN_TYPE || t-1 >  numElementsInArray(nodeTypeNames))return String::empty;
    return nodeTypeNames[(int)t-1];
}

String NodeFactory::nodeToString(NodeBase * n) {
    return nodeTypeToString((NodeType)n->nodeTypeUID);
}
