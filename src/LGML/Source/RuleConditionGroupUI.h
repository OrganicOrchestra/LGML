/*
  ==============================================================================

    RuleConditionGroupGroupUI.h
    Created: 9 May 2016 3:03:26pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULEConditionGroupGROUPUI_H_INCLUDED
#define RULEConditionGroupGROUPUI_H_INCLUDED

#include "RuleConditionGroup.h"

class RuleConditionGroupUI : public Component
{
public:
	RuleConditionGroupUI(RuleConditionGroup * conditionGroup);
	virtual ~RuleConditionGroupUI();

	RuleConditionGroup * conditionGroup;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleConditionGroupUI)

};


#endif  // RULEConditionGroupGROUPUI_H_INCLUDED
