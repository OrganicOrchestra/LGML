/*
 ==============================================================================

 NodeBaseUI.cpp
 Created: 3 Mar 2016 11:52:50pm
 Author:  bkupe

 ==============================================================================
 */



#include "NodeBaseUI.h"
#include "NodeBase.h"
#include "NodeManagerUI.h"
#include "ConnectorComponent.h"
#include "FloatSliderUI.h"





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

