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


#include "DataInNode.h"
#include "DataInNodeUI.h"
#include "NodeBaseUI.h"


DataInNode::DataInNode() :
    NodeBase("DataIn",NodeType::DataInType)
{
    addFloatParamAndData("Test Param", 0, 0, 1);
	
}

DataInNode::~DataInNode()
{

}

inline FloatParameter * DataInNode::addFloatParamAndData(const String & name, float initialValue, float minVal, float maxVal)
{
	FloatParameter * p = addNewParameter<FloatParameter>(name, "OSC Control for " + name, initialValue, minVal, maxVal);
	addOutputData(name, Data::DataType::Number);
	dynamicParameters.add(p);
	dataInListeners.call(&DataInListener::parameterAdded, p);
	return p;
}

inline void DataInNode::removeFloatParamAndData(FloatParameter * p)
{
	removeControllable(p);
	removeOutputData(p->niceName);
	dataInListeners.call(&DataInListener::parameterRemoved, p);
}

void DataInNode::onContainerParameterChanged(Parameter * p)
{
        NodeBase::onContainerParameterChanged(p);
  // @ben string obviously can't send float in updateOutputData, so updating everything but string for now
    if(dynamic_cast<StringParameter*>(p)==nullptr){
    updateOutputData(p->niceName, p->getNormalizedValue());}
}

ConnectableNodeUI * DataInNode::createUI()
{
    return new NodeBaseUI(this,new DataInNodeContentUI);
}
