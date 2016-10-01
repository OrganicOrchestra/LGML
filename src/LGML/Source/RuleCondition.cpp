/*
  ==============================================================================

    RuleCondition.cpp
    Created: 9 May 2016 2:55:10pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleCondition.h"
#include "DebugHelpers.h"
#include "Rule.h"

RuleCondition::RuleCondition(Rule * r, RuleConditionGroup * _parent) :
	rule(r),
	parent(_parent),
	isActive(false)
{
	rule->addRuleListener(this);
}

RuleCondition::~RuleCondition()
{
	rule->removeRuleListener(this);
}

void RuleCondition::setActive(bool value)
{
	if (isActive == value) return;
	isActive = value;
	conditionListeners.call(&RuleConditionListener::conditionActivationChanged, this);
}

void RuleCondition::evaluate()
{
	bool result = evaluateInternal();
	setActive(result);
}

bool RuleCondition::evaluateInternal()
{
	return false;
}

void RuleCondition::referenceValueUpdate(Rule *, ControlVariableReference *)
{
	evaluate();
}

void RuleCondition::referenceAliasChanged(Rule *, ControlVariableReference *)
{
}

void RuleCondition::remove()
{
	conditionListeners.call(&RuleConditionListener::askForRemoveCondition, this);
}

var RuleCondition::getJSONData()
{
	return var();
}

void RuleCondition::loadJSONData(var )
{
}
