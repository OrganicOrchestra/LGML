/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
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


void ConnectableNodeContentUI::setNodeAndNodeUI (ConnectableNode* _node, ConnectableNodeUI* _nodeUI)
{
    this->node = _node;
    this->nodeUI = _nodeUI;

    init();

    if (!node->nodeSize->isOverriden)
    {
        node->nodeSize->setValue (node->nodeSize->defaultValue, false);
    }


    setSize (node->nodeSize->getX(), node->nodeSize->getY());


}

void ConnectableNodeContentUI::init()
{
    setDefaultSize (100, 100);
}

void ConnectableNodeContentUI::setDefaultSize (int w, int h)
{
    jassert (node.get());
    node.get()->nodeSize->defaultValue = Array<var> {w, h};


}
