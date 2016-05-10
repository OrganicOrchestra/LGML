/*
  ==============================================================================

    RuleConditionGroup.h
    Created: 9 May 2016 2:56:20pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULECONDITIONGROUP_H_INCLUDED
#define RULECONDITIONGROUP_H_INCLUDED

#include "RuleCondition.h"

class RuleConditionGroup;

class  RuleConditionGroupListener
{
public:
	virtual ~RuleConditionGroupListener() {}

	virtual void conditionAdded(RuleCondition *) {}
	virtual void conditionRemoved(RuleCondition *) {}

	virtual void conditionGroupAdded(RuleConditionGroup *) {}
	virtual void conditionGroupRemoved(RuleConditionGroup *) {}

	virtual void conditionGroupActivationChanged(RuleConditionGroup *) {}
};


class RuleConditionGroup : public RuleCondition::RuleConditionListener, public RuleConditionGroupListener
{
public:
	enum Operator { AND, OR };

	RuleConditionGroup(RuleConditionGroup * _parent);
	virtual ~RuleConditionGroup();

	RuleConditionGroup * parent;

	OwnedArray<RuleCondition> conditions;
	OwnedArray<RuleConditionGroup> conditionGroups;
	
	RuleCondition * addCondition();
	void removeCondition(RuleCondition *);

	RuleConditionGroup * addConditionGroup();
	void removeConditionGroup(RuleConditionGroup *);

	

	ListenerList<RuleConditionGroupListener> conditionGroupListeners;
	void addConditionGroupListener(RuleConditionGroupListener* newListener) { conditionGroupListeners.add(newListener); }
	void removeConditionGroupListener(RuleConditionGroupListener* listener) { conditionGroupListeners.remove(listener); }

	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleConditionGroup)
};


#endif  // RULECONDITIONGROUP_H_INCLUDED
