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

LooperNode::LooperNode(NodeManager * nodeManager,uint32 nodeId) :NodeBase(nodeManager,nodeId,"Looper",new Looper(this)) {

}

NodeBaseUI * LooperNode::createUI(){
    return new NodeBaseUI(this, new LooperNodeContentUI);
}
