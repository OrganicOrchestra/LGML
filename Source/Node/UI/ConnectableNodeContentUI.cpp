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

#if !ENGINE_HEADLESS

#include "ConnectableNodeContentUI.h"
#include "ConnectableNodeUI.h"

ConnectableNodeContentUI::ConnectableNodeContentUI()
{
    node = nullptr;
    nodeUI = nullptr;
    setMouseCursor(MouseCursor::ParentCursor);
}

ConnectableNodeContentUI::~ConnectableNodeContentUI()
{
}


void ConnectableNodeContentUI::setNodeAndNodeUI (ConnectableNode* _node, ConnectableNodeUI* _nodeUI)
{
    this->node = _node;
    this->nodeUI = _nodeUI;

    init();




}

void ConnectableNodeContentUI::init()
{
    setDefaultSize (100, 100);
}

void ConnectableNodeContentUI::setDefaultSize (int w, int h)
{

    nodeUI->nodeSize->setNewDefault( Array<var> {w, h},false);


}
void ConnectableNodeContentUI::setDefaultPosition (int x, int y)
{

    nodeUI->nodePosition->setNewDefault( Array<var> {x, y},false);
    nodeUI->nodeMinimizedPosition->setNewDefault( Array<var> {x, y},false);
    
    
}

String ConnectableNodeContentUI::getTooltip(){
    return nodeUI->getTooltip();
};


#endif
