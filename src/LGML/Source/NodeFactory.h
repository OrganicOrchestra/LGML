/*
 ==============================================================================

 NodeFactory.h
 Created: 4 Mar 2016 10:56:58am
 Author:  bkupe

 ==============================================================================
 */

#ifndef NODEFACTORY_H_INCLUDED
#define NODEFACTORY_H_INCLUDED

#include "DummyNode.h"
#include "AudioMixerNode.h"
#include "DataInNode.h"
#include "LooperNode.h"
#include "SpatNode.h"
#include "VSTNode.h"
#include "AudioInNode.h"
#include "AudioOutNode.h"

class NodeManager;

const static String nodeTypeNames[] = { "Dummy","AudioMixer","DataIn","Spat","Looper","VST","AudioIn","AudioOut" };

class NodeFactory
{
public:
    enum NodeType
    {
        Dummy,
        AudioMixer,
        DataIn,
        Spat,
        Looper,
        VST,
        AudioIn,
        AudioOut,
        UNKNOWN // has to be last
    };

    NodeFactory()
    {

    }

    ~NodeFactory()
    {

    }

	NodeBase * createNode(NodeManager * nodeManager, NodeType nodeType, uint32 nodeId = 0);

	static PopupMenu * getNodeTypesMenu(int menuIdOffset = 0);

	static NodeType getTypeFromString(const String & s);

	static String nodeTypeToString(NodeType t);
	static String nodeToString(NodeBase *  n);

};


#endif  // NODEFACTORY_H_INCLUDED
