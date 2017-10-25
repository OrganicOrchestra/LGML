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


#include "NodeContainerContentUI.h"
#include "../../Manager/UI/NodeManagerUI.h"

#include "../../../Controllable/Parameter/UI/ParameterUI.h"


NodeContainerContentUI::NodeContainerContentUI() :
    ConnectableNodeContentUI(),
    editContainerBT ("Edit Container"),
    addUserParamBT ("Add Param Proxy")
{
    addAndMakeVisible (&editContainerBT);
    editContainerBT.addListener (this);

    addAndMakeVisible (&addUserParamBT);
    addUserParamBT.addListener (this);

    setSize (250, 100);

}

NodeContainerContentUI::~NodeContainerContentUI()
{
    nodeContainer->removeNodeContainerListener (this);
}

void NodeContainerContentUI::resized()
{

    Rectangle<int> r = getLocalBounds().reduced (5);

    editContainerBT.setBounds (r.removeFromTop (20));
    r.removeFromTop (5);

    addUserParamBT.setBounds (r.removeFromTop (20));
    r.removeFromTop (10);



}


void NodeContainerContentUI::init()
{
    NodeContainer* nc = dynamic_cast<NodeContainer*> (node.get());

    if (!nc) {jassertfalse; return;}

    nodeContainer = nc;
    nodeContainer->addNodeContainerListener (this);


}



void NodeContainerContentUI::buttonClicked (Button* b)
{
    if (b == &editContainerBT)
    {
        NodeManagerUI* nmui  = findParentComponentOfClass<NodeManagerUI>();

        if (nmui != nullptr)
        {
            nmui->setCurrentViewedContainer (nodeContainer);
        }
    }
}
