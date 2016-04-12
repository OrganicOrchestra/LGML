/*
  ==============================================================================

    NodeBaseContentUI.cpp
    Created: 8 Mar 2016 5:53:45pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeBaseContentUI.h"



  NodeBaseContentUI::NodeBaseContentUI()
  {
      node = nullptr;
      nodeUI = nullptr;
  }

  NodeBaseContentUI::~NodeBaseContentUI()
  {
  }


  void NodeBaseContentUI::setNodeAndNodeUI(NodeBase * _node, NodeBaseUI * _nodeUI)
  {
      this->node = _node;
      this->nodeUI = _nodeUI;

      init();
  }

  void NodeBaseContentUI::init()
  {
  }
