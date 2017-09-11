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


#include "JsNode.h"

#include "JsNodeUI.h"
#include "../../Engine.h"


ConnectableNodeUI * JsNode::createUI()
{

    NodeBaseUI * ui = new NodeBaseUI(this,new JsNodeUI());
    return ui;

}



JsNode::JsNode() :NodeBase("JsNode", NodeType::JsNodeType, false), JsEnvironment("node.jsNode", this) {
	canHavePresets = false;
	
	setPreferedNumAudioInput(0);
	setPreferedNumAudioOutput(0);
}

void JsNode::clearNamespace() {
	JsEnvironment::clearNamespace();
	for (auto & p : jsParameters) {
		removeControllable(p);
	}
	jsParameters.clear();


}

void JsNode::buildLocalEnv() {
	static  Identifier addIntParameterIdentifier("addIntParameter");
	static Identifier addFloatParameterIdentifier("addFloatParameter");
	static Identifier addStringParameterIdentifier("addStringParameter");
	static Identifier addBoolParameterIdentifier("addBoolParameter");
	static Identifier addTriggerIdentifier("addTrigger");

	DynamicObject d;
	d.setProperty(jsPtrIdentifier, (int64)this);
	d.setMethod(addIntParameterIdentifier, JsNode::addIntParameter);
	d.setMethod(addFloatParameterIdentifier, JsNode::addFloatParameter);
	d.setMethod(addStringParameterIdentifier, JsNode::addStringParameter);
	d.setMethod(addBoolParameterIdentifier, JsNode::addBoolParameter);
	d.setMethod(addTriggerIdentifier, JsNode::addTriggerParameter);


	setLocalNamespace(d);

}


void JsNode::onContainerParameterChanged(Parameter * p) {
	NodeBase::onContainerParameterChanged(p);

}

var JsNode::addIntParameter(const var::NativeFunctionArgs & a) {

	JsNode * jsNode = getObjectPtrFromJS<JsNode>(a);
	if (a.numArguments<5) {
		LOG("!!! wrong number of arg for addIntParameter");
		return var::undefined();
	};
	jsNode->jsParameters.add(jsNode->ParameterContainer::addNewParameter<IntParameter>(a.arguments[0], a.arguments[1], a.arguments[2], a.arguments[3], a.arguments[4]));

	return var::undefined();
}

var JsNode::addFloatParameter(const var::NativeFunctionArgs & a) {

	JsNode * jsNode = getObjectPtrFromJS<JsNode>(a);
	if (a.numArguments<5) {
		LOG("!!! wrong number of arg for addFloatParameter");
		return var::undefined();
	};

	jsNode->jsParameters.add(jsNode->ParameterContainer::addNewParameter<FloatParameter>(a.arguments[0], a.arguments[1], a.arguments[2], a.arguments[3], a.arguments[4]));

	return var::undefined();
}

var JsNode::addStringParameter(const var::NativeFunctionArgs & a) {

	JsNode * jsNode = getObjectPtrFromJS<JsNode>(a);
	if (a.numArguments<3) {
		LOG("!!! wrong number of arg for addStringParameter");
		return var::undefined();
	};
	jsNode->jsParameters.add(jsNode->ParameterContainer::addNewParameter<StringParameter>(a.arguments[0], a.arguments[1], a.arguments[2]));

	return var::undefined();
}

var JsNode::addBoolParameter(const var::NativeFunctionArgs & a) {

	JsNode * jsNode = getObjectPtrFromJS<JsNode>(a);
	if (a.numArguments<3) {
		LOG("!!! wrong number of arg for addStringParameter");
		return var::undefined();
	};
	jsNode->jsParameters.add(jsNode->ParameterContainer::addNewParameter<BoolParameter>(a.arguments[0], a.arguments[1], a.arguments[2]));

	return var::undefined();
}

var JsNode::addTriggerParameter(const var::NativeFunctionArgs & a) {

	JsNode * jsNode = getObjectPtrFromJS<JsNode>(a);
	if (a.numArguments<2) {
		LOG("!!! wrong number of arg for addTrigger");
		return var::undefined();
	};
	jsNode->jsParameters.add(jsNode->ParameterContainer::addNewParameter<Trigger>(a.arguments[0], a.arguments[1]));

	return var::undefined();
}
