/*
  ==============================================================================

    RuleCondition.cpp
    Created: 9 May 2016 2:55:10pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleCondition.h"


RuleCondition::RuleCondition(RuleConditionGroup * _parent) :
	parent(_parent),
	isActive(false)
{
}

RuleCondition::~RuleCondition()
{
}

void RuleCondition::setActive(bool value)
{
	if (isActive == value) return;
	isActive = value;
	conditionListeners.call(&RuleConditionListener::conditionActivationChanged, this);
}

void RuleCondition::process()
{
	//check condition and setActive
}

void RuleCondition::remove()
{
	conditionListeners.call(&RuleConditionListener::askForRemoveCondition, this);
}
