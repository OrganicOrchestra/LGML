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
	enum ActivationType
	{
		OnActivate,
		OnDeactivate,
		WhileActivated,
		WhileDeactivated
	};

	Rule(const String &name);
	virtual ~Rule();
	
	StringParameter * nameParam;
	BoolParameter * enabledParam;
	BoolParameter * isActiveParam;

	ActivationType activationType;

	ScopedPointer<RuleConditionGroup> rootConditionGroup;
	OwnedArray<RuleConsequence> consequences;

	void addConsequence();
	void removeConsequence(RuleConsequence *);

	void onContainerParameterChanged(Parameter * p) override;
	void remove();


	class  RuleListener
	{
	public:
		virtual ~RuleListener() {}

		virtual void askForRemoveRule(Rule *) {}
		virtual void ruleActivationChanged(Rule *) {}

		virtual void consequenceAdded(RuleConsequence *) {}
		virtual void consequenceRemoved(RuleConsequence *) {}
	};

	ListenerList<RuleListener> ruleListeners;
	void addRuleListener(RuleListener* newListener) { ruleListeners.add(newListener); }
	void removeRuleListener(RuleListener* listener) { ruleListeners.remove(listener); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Rule)
};



#endif  // RULE_H_INCLUDED
