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
