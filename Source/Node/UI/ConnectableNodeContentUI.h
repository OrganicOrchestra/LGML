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


#ifndef ConnectableNodeCONTENTUI_H_INCLUDED
#define ConnectableNodeCONTENTUI_H_INCLUDED
#pragma once

#include "../../JuceHeaderUI.h"//keep
#include "../../UI/Style.h"
class ConnectableNodeUI;
class ConnectableNode;

class ConnectableNodeContentUI : public juce::Component
{
public:
    ConnectableNodeContentUI();
    virtual ~ConnectableNodeContentUI();

    ConnectableNodeUI* nodeUI;
    WeakReference<ConnectableNode> node;
    void setDefaultSize (int w, int h);

    virtual void setNodeAndNodeUI (ConnectableNode* node, ConnectableNodeUI* nodeUI);
    virtual void init();



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConnectableNodeContentUI)
};

#endif  // ConnectableNodeCONTENTUI_H_INCLUDED
