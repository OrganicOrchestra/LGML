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


#ifndef DATAINNODEUI_H_INCLUDED
#define DATAINNODEUI_H_INCLUDED

#include "ParameterUI.h"
#include "DataInNode.h"
#include "NodeBaseContentUI.h"

//==============================================================================
/*
*/
class DataInNodeContentUI    : public NodeBaseContentUI, DataInNode::DataInListener
{
public:
    DataInNodeContentUI();
    ~DataInNodeContentUI();

    DataInNode * dataInNode;

    OwnedArray<ParameterUI> parameterUIs;

    void paint (Graphics&)override;
    void resized()override;

    void init() override;

    //helpers
    void addUIForParameter(Parameter * p);
    void addParameterUI(ParameterUI *pui);

private:

    // Inherited via Listener
    virtual void parameterAdded(Parameter *) override;
    virtual void parameterRemoved(Parameter *) override;

    ParameterUI * getUIForParameter(Parameter * p)
    {
        for (auto &pui : parameterUIs)
        {
            if (pui->parameter == p) return pui;
        }

        return nullptr;
    }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataInNodeContentUI)
};


#endif  // DATAINNODEUI_H_INCLUDED
