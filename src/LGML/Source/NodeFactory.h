/*
 ==============================================================================

 NodeFactory.h
 Created: 4 Mar 2016 10:56:58am
 Author:  bkupe

 ==============================================================================
 */

#ifndef NODEFACTORY_H_INCLUDED
#define NODEFACTORY_H_INCLUDED

#include "JuceHeader.h"
class NodeBase;
class NodeManager;

const static String nodeTypeNames[] = {"ContainerIn","ContainerOut", "Container", "Dummy","AudioMixer","DataIn","Spat","Looper","VST","AudioDeviceIn","AudioDeviceOut" };

//Added type to not mess with class names
enum NodeType
{
    UNKNOWN_TYPE =0,
	ContainerInType,
	ContainerOutType,

	ContainerType,
    DummyType ,
    AudioMixerType,
    DataInType,
    SpatType,
    LooperType,
    VSTType,
    AudioDeviceInType,
    AudioDeviceOutType
};

class NodeFactory
{
public:
	NodeFactory();
	~NodeFactory();
	
	const static int userCanAddStartType = (int)ContainerType;

    static NodeBase * createNode(NodeType nodeType);

    static PopupMenu * getNodeTypesMenu(int menuIdOffset = 0);

    static NodeType getTypeFromString(const String & s);

    static String nodeTypeToString(NodeType t);
    static String nodeToString(NodeBase *  n);

};


#endif  // NODEFACTORY_H_INCLUDED
