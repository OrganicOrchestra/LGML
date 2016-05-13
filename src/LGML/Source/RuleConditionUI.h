/*
  ==============================================================================

    RuleConditionUI.h
    Created: 9 May 2016 3:03:17pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULECONDITIONUI_H_INCLUDED
#define RULECONDITIONUI_H_INCLUDED

#include "RuleCondition.h"

class RuleConditionUI : 
	public Component,
	public RuleCondition::RuleConditionListener
{
public:
	RuleConditionUI(RuleCondition * condition);
	virtual ~RuleConditionUI();

	RuleCondition * condition;

	void paint(Graphics &g) override;

	void conditionActivationChanged(RuleCondition *) override;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleConditionUI)

};

#endif  // RULECONDITIONUI_H_INCLUDED
