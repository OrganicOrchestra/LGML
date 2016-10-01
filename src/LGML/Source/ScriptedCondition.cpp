/*
  ==============================================================================

    ScriptedCondition.cpp
    Created: 13 May 2016 6:40:50pm
    Author:  bkupe

  ==============================================================================
*/

#include "ScriptedCondition.h"
#include "Rule.h"
#include "JsHelpers.h"

ScriptedCondition::ScriptedCondition(Rule *r) :
	RuleCondition(r,nullptr),
	JsEnvironment("Rule.Condition")
{
	codeDocument.insertText(0, "function evaluate(){\n\treturn false;\n}");
	reloadScript();

}

ScriptedCondition::~ScriptedCondition()
{
}

bool ScriptedCondition::evaluateInternal()
{
	//Evaluate script
	Array<var> args;
    static const Identifier evalIdentifier("evaluate");
	var result = callFunctionFromIdentifier(evalIdentifier, args);
	return (bool)result;
}

void ScriptedCondition::buildLocalEnv()
{
	DynamicObject obj;
	obj.setProperty(jsPtrIdentifier, (int64)this);
	for (auto &r : rule->references)
	{
		if (r->currentVariable == nullptr) continue;
		obj.setProperty(r->alias->stringValue(), r->currentVariable->parameter->createDynamicObject());
	}
	setLocalNamespace(obj);
}

var ScriptedCondition::getJSONData()
{
	DynamicObject * d = new DynamicObject();
	var data(d);

	d->setProperty("script", codeDocument.getAllContent());

	return data;
}

void ScriptedCondition::loadJSONData(var data)
{
	codeDocument.replaceAllContent(data.getDynamicObject()->getProperty("script"));
	codeDocument.clearUndoHistory();
	reloadScript();
}

void ScriptedCondition::referenceAliasChanged(Rule *, ControlVariableReference *)
{
	buildLocalEnv();
	reloadScript();
}


void ScriptedCondition::reloadScript()
{
	loadScriptContent(codeDocument.getAllContent());
}
