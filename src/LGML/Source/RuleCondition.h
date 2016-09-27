/*
  ==============================================================================

    RuleCondition.h
    Created: 9 May 2016 2:55:10pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULECONDITION_H_INCLUDED
#define RULECONDITION_H_INCLUDED


#include "Rule.h"//keep

class RuleCondition :
	public RuleListener
{
public:
	RuleCondition(Rule * r, RuleConditionGroup * parent);
	virtual ~RuleCondition();

	Rule * rule;
	RuleConditionGroup * parent;

	bool isActive;
	void setActive(bool value);

	virtual void evaluate();
	virtual bool evaluateInternal();

	virtual void referenceValueUpdate(Rule *, ControlVariableReference *) override;
	virtual void referenceAliasChanged(Rule *, ControlVariableReference *) override;

	void remove();

	virtual var getJSONData();
	virtual void loadJSONData(var data);

	ListenerList<RuleConditionListener> conditionListeners;
	void addConditionListener(RuleConditionListener* newListener) { conditionListeners.add(newListener); }
	void removeConditionListener(RuleConditionListener* listener) { conditionListeners.remove(listener); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleCondition)
};


#endif  // RULECONDITION_H_INCLUDED
