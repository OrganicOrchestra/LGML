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

class RuleConditionGroup
{
public:
	RuleConditionGroup();
	virtual ~RuleConditionGroup();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleConditionGroup)
};


#endif  // RULECONDITIONGROUP_H_INCLUDED
