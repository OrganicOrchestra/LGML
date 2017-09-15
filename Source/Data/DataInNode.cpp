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



REGISTER_NODE_TYPE(DataInNode)

DataInNode::DataInNode(StringRef name) :
    NodeBase(name)
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
  // @ben we pass only float here?!
    if(auto fp = dynamic_cast<FloatParameter*>(p)){
    updateOutputData(p->niceName, fp->getNormalizedValue());}
}


