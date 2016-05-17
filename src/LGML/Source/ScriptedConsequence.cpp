/*
  ==============================================================================

    ScriptedConsequence.cpp
    Created: 13 May 2016 6:40:37pm
    Author:  bkupe

  ==============================================================================
*/

#include "ScriptedConsequence.h"
#include "ScriptedConsequenceUI.h"

ScriptedConsequence::ScriptedConsequence(Rule * r) :
	RuleConsequence(r),
	JsEnvironment("Rule.Consequence")
{
	codeDocument.insertText(0, "function onActive(){\n\n}\n\nfunction onInactive(){\n\n}\n\nfunction whileActive(){\n\n}\n\nfunction whileInactive(){\n\n}\n\n");
	reloadScript();
}

ScriptedConsequence::~ScriptedConsequence()
{
}

void ScriptedConsequence::run()
{
	
	RuleConsequence::run();

	Array<var> args;
	callFunctionFromIdentifier(Identifier(rule->isActive()?"whileActive":"whileInactive"), args);
}

void ScriptedConsequence::buildLocalEnv()
{
	DynamicObject obj;

	obj.setProperty(ptrIdentifier, (int64)this);
	for (auto &r : references)
	{
		if (r->referenceParam == nullptr) continue;

		obj.setProperty(r->alias->stringValue(), r->referenceParam->createDynamicObject());
	}

	setLocalNamespace(obj);
}

void ScriptedConsequence::reloadScript()
{
	loadScriptContent(codeDocument.getAllContent());
}

void ScriptedConsequence::currentReferenceChanged(ControlVariableReference * cvr, ControlVariable * o, ControlVariable * n)
{
	RuleConsequence::currentReferenceChanged(cvr, o, n);
	reloadScript();
}

void ScriptedConsequence::referenceAliasChanged(ControlVariableReference *)
{
	reloadScript();
}

void ScriptedConsequence::ruleActivationChanged(Rule * r)
{
	RuleConsequence::ruleActivationChanged(r);

	Array<var> args;
	callFunctionFromIdentifier(Identifier(rule->isActive() ? "onActive" : "onInactive"), args);
}

RuleConsequenceUI * ScriptedConsequence::createUI()
{
	return new ScriptedConsequenceUI(this);
}

