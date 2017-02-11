/*
  ==============================================================================

    JsNode.cpp
    Created: 28 May 2016 2:00:46pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "JsNode.h"

#include "JsNodeUI.h"
#include "Engine.h"
extern Engine * getEngine();

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
	d.setMethod(addTriggerIdentifier, JsNode::addTrigger);


	setLocalNamespace(d);

}

void JsNode::newJsFileLoaded(){
    
    
}

void JsNode::onContainerParameterChanged(Parameter * p) {
	NodeBase::onContainerParameterChanged(p);

}

var JsNode::addIntParameter(const var::NativeFunctionArgs & a) {

	JsNode * jsNode = getObjectPtrFromJS<JsNode>(a);
	if (a.numArguments<5) {
		LOG("wrong number of arg for addIntParameter");
		return var::undefined();
	};
	jsNode->jsParameters.add(jsNode->ControllableContainer::addIntParameter(a.arguments[0], a.arguments[1], a.arguments[2], a.arguments[3], a.arguments[4]));

	return var::undefined();
}

var JsNode::addFloatParameter(const var::NativeFunctionArgs & a) {

	JsNode * jsNode = getObjectPtrFromJS<JsNode>(a);
	if (a.numArguments<5) {
		LOG("wrong number of arg for addFloatParameter");
		return var::undefined();
	};

	jsNode->jsParameters.add(jsNode->ControllableContainer::addFloatParameter(a.arguments[0], a.arguments[1], a.arguments[2], a.arguments[3], a.arguments[4]));

	return var::undefined();
}

var JsNode::addStringParameter(const var::NativeFunctionArgs & a) {

	JsNode * jsNode = getObjectPtrFromJS<JsNode>(a);
	if (a.numArguments<3) {
		LOG("wrong number of arg for addStringParameter");
		return var::undefined();
	};
	jsNode->jsParameters.add(jsNode->ControllableContainer::addStringParameter(a.arguments[0], a.arguments[1], a.arguments[2]));

	return var::undefined();
}

var JsNode::addBoolParameter(const var::NativeFunctionArgs & a) {

	JsNode * jsNode = getObjectPtrFromJS<JsNode>(a);
	if (a.numArguments<3) {
		LOG("wrong number of arg for addStringParameter");
		return var::undefined();
	};
	jsNode->jsParameters.add(jsNode->ControllableContainer::addBoolParameter(a.arguments[0], a.arguments[1], a.arguments[2]));

	return var::undefined();
}

var JsNode::addTrigger(const var::NativeFunctionArgs & a) {

	JsNode * jsNode = getObjectPtrFromJS<JsNode>(a);
	if (a.numArguments<2) {
		LOG("wrong number of arg for addTrigger");
		return var::undefined();
	};
	jsNode->jsParameters.add(jsNode->ControllableContainer::addTrigger(a.arguments[0], a.arguments[1]));

	return var::undefined();
}
