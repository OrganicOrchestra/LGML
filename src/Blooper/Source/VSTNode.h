/*
  ==============================================================================

    VSTNode.h
    Created: 2 Mar 2016 8:37:24pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef VSTNODE_H_INCLUDED
#define VSTNODE_H_INCLUDED


#include "NodeBase.h"

class VSTNode : public NodeBase
{

public:
	VSTNode(NodeManager * nodeManager,uint32 nodeId) :NodeBase(nodeManager,nodeId) {
        formatManager.addDefaultFormats();
    
    }

    
    
    NodeBaseUI * createUI()override;

    
    
    AudioPluginFormatManager formatManager;

    KnownPluginList knownPluginList;
    KnownPluginList::SortMethod pluginSortMethod;
    
    
    
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VSTNode)
};



#endif  // VSTNODE_H_INCLUDED
