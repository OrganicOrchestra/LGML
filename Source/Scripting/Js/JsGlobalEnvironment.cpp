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

#include "JsGlobalEnvironment.h"
#include "../../Utils/DebugHelpers.h"
#include "../../Time/TimeManager.h"
#include "../../Node/Manager/NodeManager.h"
#include "../../Controller/ControllerManager.h"

#include "JsHelpers.h"
juce_ImplementSingleton(JsGlobalEnvironment);

JsGlobalEnvironment::JsGlobalEnvironment()
{
    env = new DynamicObject();
    static const Identifier jsPostIdentifier("post");
    static const Identifier jsGetMillisIdentifier("getMillis");
    getEnv()->setMethod(jsPostIdentifier, JsGlobalEnvironment::post);
    getEnv()->setMethod(jsGetMillisIdentifier, JsGlobalEnvironment::getMillis);
    // default in global namespace
    linkToControllableContainer("time", TimeManager::getInstance());
    linkToControllableContainer("node", NodeManager::getInstance());
    linkToControllableContainer("controllers", ControllerManager::getInstance());
}

JsGlobalEnvironment::~JsGlobalEnvironment()
{
    clear();
    JsHelpers::JsPtrStore::deleteInstance();
}

void JsGlobalEnvironment::removeNamespace(const String &ns) { JsHelpers::removeNamespaceFromObject(ns, getEnv().get()); }

void JsGlobalEnvironment::clear()
{
    if (JsHelpers::JsPtrStore::i())
        JsHelpers::JsPtrStore::i()->clear();
}
DynamicObject::Ptr JsGlobalEnvironment::getNamespaceObject(const String &ns) { return JsHelpers::getNamespaceFromObject(ns, getEnv().get()); }

DynamicObject::Ptr JsGlobalEnvironment::getEnv() { return env.getDynamicObject(); }

var JsGlobalEnvironment::post(const juce::var::NativeFunctionArgs &a)
{
    for (int i = 0; i < a.numArguments; i++)
    {
        LOG(a.arguments[i].toString());
    }

    return var();
}

var JsGlobalEnvironment::getMillis(const juce::var::NativeFunctionArgs & /*a*/)
{
    return var((int)Time::getMillisecondCounter());
}
