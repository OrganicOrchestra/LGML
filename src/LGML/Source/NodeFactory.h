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
#include "OSC2DataNode.h"
#include "LooperNode.h"
#include "SpatNode.h"
#include "VSTNode.h"
#include "AudioInNode.h"
#include "AudioOutNode.h"

class NodeManager;

const static String nodeTypeNames[] = { "Dummy","AudioMixer","OSC2Data","Spat","Looper","VST","AudioIn","AudioOut" };

class NodeFactory
{
public:
    enum NodeType
    {
        Dummy,
        AudioMixer,
		OSC2Data,
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

    NodeBase * createNode(NodeManager * nodeManager,NodeType nodeType, uint32 nodeId = 0)
    {
        NodeBase * n = nullptr;

        switch (nodeType)
        {
            case Dummy:
                n = new DummyNode(nodeManager,nodeId);
                break;

            case AudioMixer:
                n = new AudioMixerNode(nodeManager,nodeId);
                break;

			case OSC2Data:
				n = new OSC2DataNode(nodeManager, nodeId);
				break;

            case Spat:
                n = new SpatNode(nodeManager,nodeId);
                break;
            case Looper:
                n = new LooperNode(nodeManager,nodeId);
                break;
            case VST:
                n = new VSTNode(nodeManager,nodeId);
                break;

            case AudioIn:
                n = new AudioInNode(nodeManager,nodeId);
                break;

            case AudioOut:
                n = new AudioOutNode(nodeManager,nodeId);
                break;
            case UNKNOWN:
                DBG("NodeFactory : not found type for node");
                return nullptr;
            default:
                jassert(false);
                break;
        }
        n->nodeTypeEnum = (int)nodeType;

        return n;
    }

    static PopupMenu * getNodeTypesMenu(int menuIdOffset = 0)
    {
        PopupMenu * p = new PopupMenu();
        for (int i = 0; i < numElementsInArray(nodeTypeNames);i++)
        {
            p->addItem(menuIdOffset + i+1, nodeTypeNames[i]);
        }

        return p;
    }

    static NodeType getTypeFromString(const String & s){
        for (int i = 0; i < numElementsInArray(nodeTypeNames);i++)
        {
            if(s==nodeTypeNames[i]){return NodeType(i);}
        }
        return UNKNOWN;
    }

    static String nodeTypeToString(NodeType t){
        if(t<0 || t > UNKNOWN)return String::empty;
        return nodeTypeNames[(int)t];
    }
    static String nodeToString(NodeBase *  n){
        int t = n->nodeTypeEnum;
        if(t<0 || t > UNKNOWN)return String::empty;
        return nodeTypeNames[(int)t];
    }

};


#endif  // NODEFACTORY_H_INCLUDED
