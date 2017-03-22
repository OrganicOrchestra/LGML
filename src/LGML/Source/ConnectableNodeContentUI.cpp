/*
  ==============================================================================

    ConnectableNodeContentUI.cpp
    Created: 8 Mar 2016 5:53:45pm
    Author:  bkupe

  ==============================================================================
*/

#include "ConnectableNodeContentUI.h"
#include "ConnectableNodeUI.h"

ConnectableNodeContentUI::ConnectableNodeContentUI()
{
    node = nullptr;
    nodeUI = nullptr;

}

ConnectableNodeContentUI::~ConnectableNodeContentUI()
{
}


void ConnectableNodeContentUI::setNodeAndNodeUI(ConnectableNode * _node, ConnectableNodeUI * _nodeUI)
{
    this->node = _node;
    this->nodeUI = _nodeUI;

    init();
  if(!node->nodeHeight->isOverriden){
    node->nodeWidth->setValue(node->nodeWidth->defaultValue,false);
  }
  if(!node->nodeHeight->isOverriden){
    node->nodeHeight->setValue(node->nodeHeight->defaultValue,false);
  }

    setSize(node->nodeWidth->intValue(),node->nodeHeight->intValue());


}

void ConnectableNodeContentUI::init()
{
  setDefaultSize(100, 100);
}

void ConnectableNodeContentUI::setDefaultSize(int w,int h){
  jassert(node.get());
  node.get()->nodeWidth->defaultValue=w;
  node.get()->nodeHeight->defaultValue=h;

}
