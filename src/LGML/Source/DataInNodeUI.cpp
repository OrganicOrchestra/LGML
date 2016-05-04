/*
 ==============================================================================

 DataInNodeUI.cpp
 Created: 19 Apr 2016 10:39:16am
 Author:  bkupe

 ==============================================================================
 */


#include "DataInNodeUI.h"
#include "FloatSliderUI.h"

//==============================================================================
DataInNodeContentUI::DataInNodeContentUI()
{

}

DataInNodeContentUI::~DataInNodeContentUI()
{
    o2dNode->removeO2DListener(this);
}

void DataInNodeContentUI::paint(Graphics&)
{

}

void DataInNodeContentUI::resized()
{
    Rectangle<int> r = getLocalBounds().reduced(5);
    for (auto &pui : parameterUIs)
    {
        pui->setBounds(r.removeFromTop(20));
        r.removeFromTop(10);
    }
}

void DataInNodeContentUI::init()
{
    setSize(150, 200);

    o2dNode = (DataInNode *)node;
    o2dNode->addO2DListener(this);

    for (auto &p : o2dNode->dynamicParameters)
    {
        addUIForParameter(p);
    }
}

void DataInNodeContentUI::addUIForParameter(Parameter * p)
{
    switch (p->type)
    {
        case Parameter::Type::FLOAT:
            addParameterUI(((FloatParameter *)p)->createSlider());
            break;
        default:
            DBG("no UI for type : " << p->type);
            break;

    }

}

void DataInNodeContentUI::addParameterUI(ParameterUI * pui)
{
    parameterUIs.add(pui);
    addAndMakeVisible(pui);
    DBG("Add Parameter UI : "+getLocalBounds().toString());
    setBounds(getLocalBounds().withHeight(parameterUIs.size() * 30));
}

void DataInNodeContentUI::parameterAdded(Parameter * p)
{
    addUIForParameter(p);
}

void DataInNodeContentUI::parameterRemoved(Parameter * p)
{
    ParameterUI * pui = getUIForParameter(p);
    if (pui == nullptr) return;
    parameterUIs.removeObject(pui);
    resized();
}
