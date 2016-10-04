/*
 ==============================================================================

 NodeFactory.h
 Created: 4 Mar 2016 10:56:58am
 Author:  bkupe

 ==============================================================================
 */

#ifndef NODEFACTORY_H_INCLUDED
#define NODEFACTORY_H_INCLUDED
#pragma once
#include "JuceHeader.h"//keep

class ConnectableNode;

const static String nodeTypeNames[] = {	"ContainerIn",
										"ContainerOut", 
										"Container", 
										"Dummy",
										"AudioMixer",
										"DataIn",
										"Spat2D",
										"Looper",
										"VST",
										"AudioDeviceIn",
										"AudioDeviceOut",
										"JsNode" };

//Added type to not mess with class names
enum NodeType
{
    UNKNOWN_TYPE,
	ContainerInType,
	ContainerOutType,

	ContainerType,
    DummyType ,
    AudioMixerType,
    DataInType,
    Spat2DType,
    LooperType,
    VSTType,
    AudioDeviceInType,
    AudioDeviceOutType,
    JsNodeType
};

class NodeFactory
{
public:
	NodeFactory();
	~NodeFactory();

	const static int userCanAddStartType = ContainerType;

    static ConnectableNode * createNode(NodeType type);

    static PopupMenu * getNodeTypesMenu(int menuIdOffset = 0);

    static NodeType getTypeFromString(const String & s);
	static NodeType getTypeForIndex(int nodeTypeIndex, bool includeUserOffset);

    static String nodeTypeToString(NodeType t);
    static String nodeToString(ConnectableNode *  n);

};


#endif  // NODEFACTORY_H_INCLUDED
