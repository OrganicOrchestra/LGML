/*
  ==============================================================================

    RuleConditionGroup.cpp
    Created: 9 May 2016 2:56:20pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleConditionGroup.h"
#include "RuleCondition.h"
#include "Rule.h"

RuleConditionGroup::RuleConditionGroup(Rule * r,RuleConditionGroup * _parent) :
	rule(r),
	parent(_parent),
	groupOperator(AND)
{
}

RuleConditionGroup::~RuleConditionGroup()
{
}

RuleCondition * RuleConditionGroup::addCondition()
{
	RuleCondition * c = new RuleCondition(rule,this);
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
	RuleConditionGroup * c = new RuleConditionGroup(rule, this);
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

bool RuleConditionGroup::isActive()
{
	for (auto &c : conditions) if (!c->isActive) return false;
	for (auto &cg : conditionGroups) if (!cg->isActive()) return false;

	return true;
}

var RuleConditionGroup::getJSONData()
{
	DynamicObject * d = new DynamicObject();
	var data(d);
	d->setProperty("groupOperator", (int)groupOperator);

	var conditionsData;
	for (auto & c : conditions) conditionsData.append(c->getJSONData());
	d->setProperty("conditions", conditionsData);

	var groupsData;
	for (auto & cg : conditionGroups) groupsData.append(cg->getJSONData());
	d->setProperty("groups", groupsData);

	return data;
}

void RuleConditionGroup::loadJSONData(var data)
{
	groupOperator = (GroupOperator)(int)data.getDynamicObject()->getProperty("groupOperator");

	Array<var> * conditionsData = data.getDynamicObject()->getProperty("conditions").getArray();
	for (auto & cData : *conditionsData)
	{
		RuleCondition * c = addCondition();
		c->loadJSONData(cData);
	}

	Array<var> * groupsData = data.getDynamicObject()->getProperty("groups").getArray();
	for (auto & cgData : *groupsData)
	{
		RuleConditionGroup * cg = addConditionGroup();
		cg->loadJSONData(cgData);
	}
}
