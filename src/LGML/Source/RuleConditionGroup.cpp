/*
  ==============================================================================

    RuleConditionGroup.cpp
    Created: 9 May 2016 2:56:20pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleConditionGroup.h"


RuleConditionGroup::RuleConditionGroup(RuleConditionGroup * _parent) :
	parent(_parent)
{
}

RuleConditionGroup::~RuleConditionGroup()
  {
  }

RuleCondition * RuleConditionGroup::addCondition()
{
	RuleCondition * c = new RuleCondition(this);
	conditions.add(c);
	c->addConditionListener(this);
	conditionGroupListeners.call(&RuleConditionGroupListener::conditionAdded, c);
	return c;
}

void RuleConditionGroup::removeCondition(RuleCondition * c)
{
	conditionGroupListeners.call(&RuleConditionGroupListener::conditionRemoved, c);
	c->removeConditionListener(this);
	conditions.removeObject(c);

}

RuleConditionGroup * RuleConditionGroup::addConditionGroup()
{
	RuleConditionGroup * c = new RuleConditionGroup(this);
	conditionGroups.add(c);
	c->addConditionGroupListener(this);
	conditionGroupListeners.call(&RuleConditionGroupListener::conditionGroupAdded, c);
	return c;
}

void RuleConditionGroup::removeConditionGroup(RuleConditionGroup * c)
{
	conditionGroupListeners.call(&RuleConditionGroupListener::conditionGroupRemoved, c);
	c->removeConditionGroupListener(this);
	conditionGroups.removeObject(c);
}
