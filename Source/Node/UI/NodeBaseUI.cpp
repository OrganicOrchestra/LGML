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




#include "NodeBaseUI.h"
#include "../NodeBase.h"
#include "../Manager/UI/NodeManagerUI.h"
#include "ConnectorComponent.h"
#include "../../Controllable/Parameter/UI/SliderUI.h"





//==============================================================================
NodeBaseUI::NodeBaseUI(NodeBase * _node, NodeBaseContentUI * _contentContainer, NodeBaseHeaderUI * _headerContainer) :
ConnectableNodeUI(_node,_contentContainer, _headerContainer),
node(_node)
{
  setInterceptsMouseClicks(false, true);

}

NodeBaseUI::~NodeBaseUI()
{
}
