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
  }

  void ConnectableNodeContentUI::init()
  {
  }
