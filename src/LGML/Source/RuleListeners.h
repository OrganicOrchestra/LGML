/*
  ==============================================================================

    RuleListeners.h
    Created: 17 May 2016 12:47:50pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULELISTENERS_H_INCLUDED
#define RULELISTENERS_H_INCLUDED


class Rule;
class RuleConsequence;
class RuleCondition;
class RuleConditionGroup;
class ControlVariableReference;


class  RuleListener
{
public:
	virtual ~RuleListener() {}

	virtual void askForRemoveRule(Rule *) {}
	virtual void ruleActivationChanged(Rule *) {}
	virtual void ruleConditionTypeChanged(Rule *) {}

	virtual void referenceAdded(Rule *,  ControlVariableReference *) {}
	virtual void referenceRemoved(Rule *, ControlVariableReference *) {}
	virtual void referenceAliasChanged(Rule *, ControlVariableReference *) {}
	virtual void referenceValueUpdate(Rule *, ControlVariableReference *) {}

	virtual void consequenceAdded(RuleConsequence *) {}
	virtual void consequenceRemoved(RuleConsequence *) {}

};


class  RuleConditionListener
{
public:
	virtual ~RuleConditionListener() {}

	virtual void askForRemoveCondition(RuleCondition *) {};
	virtual void conditionActivationChanged(RuleCondition *) {}
};


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


class  RuleConsequenceListener
{
public:
	virtual ~RuleConsequenceListener() {}

	virtual void askForRemoveConsequence(RuleConsequence *) {};
};


#endif  // RULELISTENERS_H_INCLUDED
