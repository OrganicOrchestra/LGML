/*
  ==============================================================================

    Rule.h
    Created: 4 May 2016 5:06:55pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULE_H_INCLUDED
#define RULE_H_INCLUDED

#include "JuceHeader.h"
#include "RuleCondition.h"
#include "RuleConditionGroup.h"
#include "RuleConsequence.h"

#include "ControllableContainer.h"

class Rule : public ControllableContainer
{
public:
	Rule(const String &name);
	virtual ~Rule();
	
	StringParameter * nameParam;
	BoolParameter * enabledParam;
	BoolParameter * isActiveParam;
	bool isSelected; //for ui

	OwnedArray<RuleCondition> conditions;
	OwnedArray<RuleConditionGroup> conditionGroups;
	OwnedArray<RuleConsequence> consequences;

	void onContainerParameterChanged(Parameter * p) override;

	void setSelected(bool value);

	void select();
	void remove();


	class  RuleListener
	{
	public:
		/** Destructor. */
		virtual ~RuleListener() {}

		virtual void askForSelectRule(Rule *) {}
		virtual void askForRemoveRule(Rule *) {}

		virtual void ruleSelectionChanged(Rule *) {}
		virtual void ruleActivationChanged(Rule *) {}
	};

	ListenerList<RuleListener> ruleListeners;
	void addRuleListener(RuleListener* newListener) { ruleListeners.add(newListener); }
	void removeRuleListener(RuleListener* listener) { ruleListeners.remove(listener); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Rule)
};



#endif  // RULE_H_INCLUDED
