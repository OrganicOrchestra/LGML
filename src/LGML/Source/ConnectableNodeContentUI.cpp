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
  if(!node->nodeSize->isOverriden){
    node->nodeSize->setValue(node->nodeSize->defaultValue,false);
  }


    setSize(node->nodeSize->getX(),node->nodeSize->getY());


}

void ConnectableNodeContentUI::init()
{
  setDefaultSize(100, 100);
}

void ConnectableNodeContentUI::setDefaultSize(int w,int h){
  jassert(node.get());
  node.get()->nodeSize->defaultValue= Array<var>{w,h};
  

}
