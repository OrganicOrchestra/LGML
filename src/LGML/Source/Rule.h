/*
  ==============================================================================

    Rule.h
    Created: 4 May 2016 5:06:55pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULE_H_INCLUDED
#define RULE_H_INCLUDED

#include "ControllableContainer.h"
#include "ControlVariableReference.h"

#include "RuleListeners.h"

class ScriptedCondition;

class Rule :
	public ControllableContainer,
	public ControlVariableReferenceListener,
	public RuleConditionListener,
	public RuleConditionGroupListener,
	public RuleConsequenceListener
{
public:
	Rule(const String &name);
	virtual ~Rule();

	enum ConditionType {NONE, SCRIPT, VISUAL };
	ConditionType conditionType;
	void setConditionType(ConditionType value);

	StringParameter * nameParam;
	BoolParameter * enabledParam;
	BoolParameter * isActiveParam;

	OwnedArray<ControlVariableReference> references;
	ScopedPointer<RuleConditionGroup> rootConditionGroup;
	ScopedPointer<ScriptedCondition> scriptedCondition;

	OwnedArray<RuleConsequence> consequences;

	ControlVariableReference * addReference();
	void removeReference(ControlVariableReference *);

	RuleConsequence * addConsequence();
	void removeConsequence(RuleConsequence *);
	void askForRemoveConsequence(RuleConsequence *) override;

	void onContainerParameterChanged(Parameter * p) override;
	void askForRemoveReference(ControlVariableReference * r) override;

	void referenceAliasChanged(ControlVariableReference * r) override;
	void referenceValueChanged(ControlVariableReference * r) override;

	void conditionActivationChanged(RuleCondition * c) override;

	bool isActive();

	void clear();

	void remove();

	virtual var getJSONData() override;
	virtual void loadJSONDataInternal(var data) override;

	ListenerList<RuleListener> ruleListeners;
	void addRuleListener(RuleListener* newListener) { ruleListeners.add(newListener); }
	void removeRuleListener(RuleListener* listener) { ruleListeners.remove(listener); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Rule)
};



#endif  // RULE_H_INCLUDED
