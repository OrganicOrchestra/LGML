/*
 ==============================================================================

 Rule.cpp
 Created: 4 May 2016 5:06:55pm
 Author:  bkupe

 ==============================================================================
 */

#include "Rule.h"
#include "RuleConsequence.h"
#include "ScriptedConsequence.h"

Rule::Rule(const String &_name) :
ControllableContainer(_name),
activationType(OnActivate),
conditionType(NONE),
rootConditionGroup(nullptr),
scriptedCondition(nullptr)
{

    nameParam = addStringParameter("Name", "Name of the rule", _name);
    enabledParam = addBoolParameter("Enabled", "Enable / Disable the rule",true);

    isActiveParam = addBoolParameter("active", "Whether the rule is active or not.\nA rule is active if its conditions are validated.", false);
    isActiveParam->isControllableFeedbackOnly = true;

    setConditionType(ConditionType::SCRIPT);
}

Rule::~Rule()
{
}

void Rule::setConditionType(ConditionType value)
{
    if (conditionType == value) return;

    switch (conditionType)
    {
        case SCRIPT:
			if(scriptedCondition != nullptr) scriptedCondition->removeConditionListener(this);
            scriptedCondition = nullptr;
            break;

        case VISUAL:
			if (rootConditionGroup != nullptr) rootConditionGroup->removeConditionGroupListener(this);
			rootConditionGroup = nullptr;

            break;
        default:
            break;
    }

    conditionType = value;

    switch (conditionType)
    {
        case SCRIPT:
            scriptedCondition = new ScriptedCondition();
			scriptedCondition->setReferences(&references);
			scriptedCondition->addConditionListener(this);
            break;

        case VISUAL:
            rootConditionGroup = new RuleConditionGroup(nullptr);
			rootConditionGroup->addConditionGroupListener(this);
            break;
        default:
            break;
    }

    ruleListeners.call(&RuleListener::ruleConditionTypeChanged, this);
}

ControlVariableReference * Rule::addReference()
{
    ControlVariableReference * cvr = new ControlVariableReference();
    references.add(cvr);
    cvr->addReferenceListener(this);
    ruleListeners.call(&RuleListener::referenceAdded, cvr);

	updateReferencesInCondition();
	updateReferencesInConsequences();

    return cvr;
}

void Rule::removeReference(ControlVariableReference * cvr)
{
    cvr->removeReferenceListener(this);
    ruleListeners.call(&RuleListener::referenceRemoved, cvr);
    references.removeObject(cvr);
	updateReferencesInCondition();
	updateReferencesInConsequences();
}

void Rule::updateReferencesInCondition()
{
	if (conditionType == SCRIPT)
	{
		scriptedCondition->setReferences(&references);
		
	}
}

void Rule::updateReferencesInConsequences()
{
	for (auto &c : consequences) c->setReferences(&references);
}

void Rule::addConsequence()
{
    RuleConsequence * c = new ScriptedConsequence(this);
    consequences.add(c);
    ruleListeners.call(&RuleListener::consequenceAdded,c);
	updateReferencesInConsequences();
}

void Rule::removeConsequence(RuleConsequence * c)
{
    ruleListeners.call(&RuleListener::consequenceRemoved, c);
    consequences.removeObject(c);
	updateReferencesInConsequences();

}

void Rule::onContainerParameterChanged(Parameter * p)
{
    if (p == nameParam)
    {
        setNiceName(nameParam->stringValue());
    }else if (p == isActiveParam)
    {
        ruleListeners.call(&RuleListener::ruleActivationChanged, this);
    }
}

void Rule::askForRemoveReference(ControlVariableReference * r)
{
    removeReference(r);
	
}

void Rule::conditionActivationChanged(RuleCondition *)
{
	isActiveParam->setValue((conditionType == SCRIPT)?scriptedCondition->isActive:rootConditionGroup->isActive());
}

bool Rule::isActive()
{
	return isActiveParam->boolValue();
}


void Rule::remove()
{
    ruleListeners.call(&RuleListener::askForRemoveRule, this);
}
