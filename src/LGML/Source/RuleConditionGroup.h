/*
  ==============================================================================

    RuleConditionGroup.h
    Created: 9 May 2016 2:56:20pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULECONDITIONGROUP_H_INCLUDED
#define RULECONDITIONGROUP_H_INCLUDED

#include "Rule.h"

class RuleConditionGroup :
	public RuleConditionListener,
	public RuleConditionGroupListener
{
public:
	enum GroupOperator { AND, OR };

	RuleConditionGroup(Rule * r, RuleConditionGroup * _parent);
	virtual ~RuleConditionGroup();

	Rule * rule;
	RuleConditionGroup * parent;

	GroupOperator groupOperator;

	OwnedArray<RuleCondition> conditions;
	OwnedArray<RuleConditionGroup> conditionGroups;

	RuleCondition * addCondition();
	void removeCondition(RuleCondition *);

	RuleConditionGroup * addConditionGroup();
	void removeConditionGroup(RuleConditionGroup *);

	bool isActive();

	virtual var getJSONData();
	virtual void loadJSONData(var data);

	ListenerList<RuleConditionGroupListener> conditionGroupListeners;
	void addConditionGroupListener(RuleConditionGroupListener* newListener) { conditionGroupListeners.add(newListener); }
	void removeConditionGroupListener(RuleConditionGroupListener* listener) { conditionGroupListeners.remove(listener); }


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleConditionGroup)
};


#endif  // RULECONDITIONGROUP_H_INCLUDED
