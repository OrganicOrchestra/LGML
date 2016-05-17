/*
  ==============================================================================

    ScriptedCondition.cpp
    Created: 13 May 2016 6:40:50pm
    Author:  bkupe

  ==============================================================================
*/

#include "ScriptedCondition.h"

ScriptedCondition::ScriptedCondition() :
	RuleCondition(nullptr),
	JsEnvironment("Rule.Condition")
{
	codeDocument.insertText(0, "function evaluate(){\n\treturn false;\n}");
	reloadScript();

}

ScriptedCondition::~ScriptedCondition()
{
}

void ScriptedCondition::setReferences(OwnedArray<ControlVariableReference> *_ref)
{
	RuleCondition::setReferences(_ref);

	buildLocalEnv(); //  update js environment
}

bool ScriptedCondition::evaluateInternal()
{
	//Evaluate script
	Array<var> args;
	var result = callFunctionFromIdentifier(Identifier("evaluate"), args);
	return (bool)result;
}

void ScriptedCondition::buildLocalEnv()
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

void ScriptedCondition::reloadScript()
{
	loadScriptContent(codeDocument.getAllContent());
}

void ScriptedCondition::currentReferenceChanged(ControlVariableReference * cvr , ControlVariable * o , ControlVariable * n)
{
	RuleCondition::currentReferenceChanged(cvr, o, n);
	reloadScript();
}

void ScriptedCondition::referenceAliasChanged(ControlVariableReference *)
{
	reloadScript();
}

