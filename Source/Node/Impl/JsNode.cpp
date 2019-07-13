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
//#include "../../Engine.h"
#include "../../Scripting/Js/JsHelpers.h"

REGISTER_NODE_TYPE (JsNode)



JsNode::JsNode (StringRef name) : NodeBase (name, false), JsEnvironment ("node.jsNode", this)
{
    _canHavePresets = false;

    setPreferedNumAudioInput (0);
    setPreferedNumAudioOutput (0);
    containSavableObjects->setValue(true,true);
}

void JsNode::clearNamespace()
{
    JsEnvironment::clearNamespace();

    for (auto& p : jsDynamicParameters)
    {
        removeControllable (p);
    }

    jsDynamicParameters.clear();


}

void JsNode::buildLocalEnv()
{
    static  Identifier addIntParameterIdentifier ("addIntParameter");
    static Identifier addFloatParameterIdentifier ("addFloatParameter");
    static Identifier addStringParameterIdentifier ("addStringParameter");
    static Identifier addBoolParameterIdentifier ("addBoolParameter");
    static Identifier addTriggerIdentifier ("addTrigger");

    DynamicObject d;

    d.setMethod (addIntParameterIdentifier, JsNode::addIntParameter);
    d.setMethod (addFloatParameterIdentifier, JsNode::addFloatParameter);
    d.setMethod (addStringParameterIdentifier, JsNode::addStringParameter);
    d.setMethod (addBoolParameterIdentifier, JsNode::addBoolParameter);
    d.setMethod (addTriggerIdentifier, JsNode::addTriggerParameter);


    setLocalNamespace (d);

}


void JsNode::onContainerParameterChanged ( ParameterBase* p)
{
    
    NodeBase::onContainerParameterChanged (p);


}

var JsNode::addIntParameter (const var::NativeFunctionArgs& a)
{

    JsNode* jsNode = castPtrFromJSEnv<JsNode> (a);

    if (a.numArguments < 5)
    {
        LOGE(juce::translate("wrong number of arg for addIntParameter"));
        return var::undefined();
    };
    auto res = jsNode->ParameterContainer::addNewParameter<IntParameter> (a.arguments[0], a.arguments[1], a.arguments[2], a.arguments[3], a.arguments[4]);
    res->isUserDefined = true;
    jsNode->jsDynamicParameters.add (res);

    return var::undefined();
}

var JsNode::addFloatParameter (const var::NativeFunctionArgs& a)
{

    auto* jsNode = castPtrFromJSEnv<JsNode> (a);

    if (a.numArguments < 5)
    {
        LOGE(juce::translate("wrong number of arg for addFloatParameter"));
        return var::undefined();
    };

    auto res = jsNode->ParameterContainer::addNewParameter<FloatParameter> (a.arguments[0], a.arguments[1], a.arguments[2], a.arguments[3], a.arguments[4]);

    res->isUserDefined = true;
    jsNode->jsDynamicParameters.add (res);
    return var::undefined();
}

var JsNode::addStringParameter (const var::NativeFunctionArgs& a)
{

    JsNode* jsNode = castPtrFromJSEnv<JsNode> (a);

    if (a.numArguments < 3)
    {
        LOGE(juce::translate("wrong number of arg for addStringParameter"));
        return var::undefined();
    };

    auto res = jsNode->ParameterContainer::addNewParameter<StringParameter> (a.arguments[0], a.arguments[1], a.arguments[2]);
    res->isUserDefined = true;
    jsNode->jsDynamicParameters.add (res);
    return var::undefined();
}

var JsNode::addBoolParameter (const var::NativeFunctionArgs& a)
{

    JsNode* jsNode = castPtrFromJSEnv<JsNode> (a);

    if (a.numArguments < 3)
    {
        LOGE(juce::translate("wrong number of arg for addStringParameter"));
        return var::undefined();
    };

    auto res = jsNode->ParameterContainer::addNewParameter<BoolParameter> (a.arguments[0], a.arguments[1], a.arguments[2]);
    res->isUserDefined = true;
    jsNode->jsDynamicParameters.add (res);
    return var::undefined();
}

var JsNode::addTriggerParameter (const var::NativeFunctionArgs& a)
{

    JsNode* jsNode = castPtrFromJSEnv<JsNode> (a);

    if (a.numArguments < 2)
    {
        LOGE(juce::translate("wrong number of arg for addTrigger"));
        return var::undefined();
    };

    auto res = jsNode->ParameterContainer::addNewParameter<Trigger> (a.arguments[0], a.arguments[1]);
    res->isUserDefined = true;
    jsNode->jsDynamicParameters.add (res);

    return var::undefined();
}

ParameterBase* JsNode::addParameterFromVar(const String& name, const var& data) {
    containSavableObjects->setValue(true,true);
    auto res = ParameterContainer::addParameterFromVar(name, data);
    if(res)jsDynamicParameters.add (res);
    return res;
}

String JsNode::getSubTypeName(){
    if(auto * fp = getJsFileParameter()){
        if(fp->getFile().exists()){
            return fp->getFile().getFileNameWithoutExtension();
        }
    }
    return "";

}
