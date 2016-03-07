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
#include "LooperNode.h"
#include "SpatNode.h"
#include "VSTNode.h"

class NodeFactory
{
public:
	enum NodeType
	{
		Dummy,
		AudioMixer,
		Spat,
		Looper,
		VST
	};

	NodeFactory()
	{

	}

	~NodeFactory()
	{

	}

	NodeBase * createNode(NodeType nodeType, uint32 nodeId = 0)
	{
		NodeBase * n;

		switch (nodeType)
		{
		case Dummy:
			n = new DummyNode(nodeId);
			break;

		case AudioMixer:
			n = new AudioMixerNode(nodeId);
			break;
		case Spat:
			n = new SpatNode(nodeId);
			break;
		case Looper:
			n = new LooperNode(nodeId);
			break;
		case VST:
			n = new VSTNode(nodeId);
			break;
		default:
			jassert(false);
			break;
		}


		return n;
	}

	static PopupMenu * getNodeTypesMenu(int menuIdOffset = 0)
	{
		PopupMenu * p = new PopupMenu();
		Array<String> nodeTypeNames = { "Dummy","AudioMixer","Spat","Looper","VST" };
		for (int i = 0; i < nodeTypeNames.size();i++)
		{
			p->addItem(menuIdOffset + i+1, nodeTypeNames[i]);
		}
		
		return p;
	}
};


#endif  // NODEFACTORY_H_INCLUDED
