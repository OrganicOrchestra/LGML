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

const static String nodeTypeNames[] = {"Container", "Dummy","AudioMixer","DataIn","Spat","Looper","VST","AudioIn","AudioOut" };

//Added type to not mess with class names
enum NodeType
{
    UNKNOWN_TYPE =0,
	ContainerType,
    DummyType ,
    AudioMixerType,
    DataInType,
    SpatType,
    LooperType,
    VSTType,
    AudioInType,
    AudioOutType

};

class NodeFactory
{
public:
	NodeFactory();
	~NodeFactory();
    static NodeBase * createNode(NodeType nodeType, uint32 nodeId = 0);

    static PopupMenu * getNodeTypesMenu(int menuIdOffset = 0);

    static NodeType getTypeFromString(const String & s);

    static String nodeTypeToString(NodeType t);
    static String nodeToString(NodeBase *  n);

};


#endif  // NODEFACTORY_H_INCLUDED
