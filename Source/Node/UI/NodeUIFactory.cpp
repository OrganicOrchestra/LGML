
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


#include "NodeUIFactory.h"

#include "../ConnectableNode.h"
#include "../UI/ConnectableNodeUI.h"



#include "../Impl/AudioDeviceInNodeUI.h"
#include "../Impl/AudioDeviceInNode.h"

#include "../Impl/AudioDeviceOutNodeUI.h"
#include "../Impl/AudioDeviceOutNode.h"

#include "../Impl/AudioMixerNodeUI.h"
#include "../Impl/AudioMixerNode.h"
//#include "../Impl/DataInNodeUI.h"

#include "../Impl/JsNodeUI.h"
#include "../Impl/JsNode.h"

#include "../Impl/ContainerInNode.h"
#include "../Impl/ContainerOutNode.h"

#include "../../Data/DataInNode.h"
#include "../../Data/DataInNodeUI.h"


#include "../Impl/DummyNode.h"
#include "../Impl/DummyNodeContentUI.h"

#include "../Impl/LooperNode.h"
#include "../Impl/LooperNodeUI.h"
#include "../NodeContainer/NodeContainer.h"
#include "../NodeContainer/UI/NodeContainerContentUI.h"
#include "../Impl/Spat2DNode.h"
#include "../Impl/Spat2DNodeUI.h"
#include "../Impl/VSTNode.h"
#include "../Impl/VSTNodeUI.h"

//#define CHKNRETURN_HEADER(p,A,B,C) if(p->getTypeId()==A::_objType){return new ConnectableNodeUI(p, B,C);}
#define CHKNRETURN(p,A,B) if(p->getTypeId()==A::_objType){return new ConnectableNodeUI(p, uip,B);}

ConnectableNodeUI* NodeUIFactory::createDefaultUI (ConnectableNode* t,ConnectableNodeUIParams *uip)
{


    CHKNRETURN (t, AudioDeviceInNode, new AudioDeviceInNodeContentUI());
    CHKNRETURN (t, AudioDeviceOutNode, new AudioDeviceOutNodeContentUI());
    CHKNRETURN (t, AudioMixerNode, new AudioMixerNodeUI); //ui->recursiveInspectionLevel = 2;
    CHKNRETURN (t, JsNode, new JsNodeUI);

    CHKNRETURN (t, DataInNode, new DataInNodeContentUI);

    CHKNRETURN (t, DummyNode, new DummyNodeContentUI);
    CHKNRETURN (t, LooperNode, new LooperNodeContentUI); //ui->recursiveInspectionLevel = 2; ui->canInspectChildContainersBeyondRecursion = false;
    CHKNRETURN (t, NodeContainer, new NodeContainerContentUI); // recursiveInspectionLevel = 1; canInspectChildContainersBeyondRecursion = true;
    CHKNRETURN (t, Spat2DNode, new Spat2DNodeContentUI);
    CHKNRETURN (t, VSTNode,  new VSTNodeContentUI); //, new VSTNodeHeaderUI);


    // set default for ContainerIn/Out
    if(t->getTypeId()==ContainerInNode::_objType){
        uip->nodePosition->setNewDefault(Array<var>({10,10}), false);
        uip->nodeMinimizedPosition->setNewDefault(Array<var>({10,10}), false);
        return new ConnectableNodeUI(t, uip,nullptr);

    }
    if(t->getTypeId()==ContainerOutNode::_objType){
        uip->nodePosition->setNewDefault(Array<var>({200,10}), false);
        uip->nodeMinimizedPosition->setNewDefault(Array<var>({200,10}), false);
        return new ConnectableNodeUI(t, uip,nullptr);

    }


    jassertfalse;
    return nullptr;


}
