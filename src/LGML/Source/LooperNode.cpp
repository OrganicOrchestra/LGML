/*
 ==============================================================================

 LooperNode.cpp
 Created: 3 Mar 2016 10:32:16pm
 Author:  bkupe

 ==============================================================================
 */

#include "LooperNode.h"
#include "LooperNodeUI.h"

#include "Looper.h"

LooperNode::LooperNode(NodeManager * nodeManager,uint32 nodeId) :
	NodeBase(nodeManager,nodeId,"Looper",new Looper(this)) 
{
	looper = dynamic_cast<Looper*>(audioProcessor);
}

NodeBaseUI * LooperNode::createUI(){
    NodeBaseUI * ui = new NodeBaseUI(this, new LooperNodeContentUI);
	ui->recursiveInspectionLevel = 1;
	return ui;
}
