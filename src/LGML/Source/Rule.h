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

	void onContainerParameterChanged(Parameter * p) override;

	OwnedArray<RuleCondition> conditions;
	OwnedArray<RuleConditionGroup> conditionGroups;

	OwnedArray<RuleConsequence> consequences;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Rule)
};



#endif  // RULE_H_INCLUDED
