/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#if !ENGINE_HEADLESS

#include "DataInNodeUI.h"
#include "../Controllable/Parameter/UI/SliderUI.h"

//==============================================================================
DataInNodeContentUI::DataInNodeContentUI()
{

}

DataInNodeContentUI::~DataInNodeContentUI()
{
    dataInNode->removeDataInListener (this);
}

void DataInNodeContentUI::paint (Graphics&)
{

}

void DataInNodeContentUI::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (5);

    for (auto& pui : parameterUIs)
    {
        pui->setBounds (r.removeFromTop (20));
        r.removeFromTop (10);
    }
}

void DataInNodeContentUI::init()
{


    dataInNode = (DataInNode*)node.get();
    dataInNode->addDataInListener (this);

    for (auto& p : dataInNode->dynamicParameters)
    {
        addUIForParameter (p);
    }

    setDefaultSize (150, 200);
}

void DataInNodeContentUI::addUIForParameter (Parameter* p)
{
    //    if
    //    {
    //        case Parameter::Type::FLOAT:
    //            addParameterUI(new FloatSliderUI((FloatParameter *)p));
    //            break;
    //        default:
    //            DBG("no UI for type : " << p->type);
    //            break;
    //
    //    }

}

void DataInNodeContentUI::addParameterUI (ParameterUI* pui)
{
    parameterUIs.add (pui);
    addAndMakeVisible (pui);
    //DBG("Add Parameter UI : "+getLocalBounds().toString());
    setBounds (getLocalBounds().withHeight (parameterUIs.size() * 30));
}

void DataInNodeContentUI::parameterAdded (Parameter* p)
{
    addUIForParameter (p);
}

void DataInNodeContentUI::parameterRemoved (Parameter* p)
{
    ParameterUI* pui = getUIForParameter (p);

    if (pui == nullptr) return;

    parameterUIs.removeObject (pui);
    resized();
}
#endif
