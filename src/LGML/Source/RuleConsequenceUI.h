/*
  ==============================================================================

    RuleConsequenceUI.h
    Created: 9 May 2016 3:03:37pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULECONSEQUENCEUI_H_INCLUDED
#define RULECONSEQUENCEUI_H_INCLUDED


#include "RuleConsequence.h"
class RuleConsequenceUI : public Component
{
public:
	RuleConsequenceUI(RuleConsequence * consequence);
	virtual ~RuleConsequenceUI();

	RuleConsequence * consequence;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleConsequenceUI)

};



#endif  // RULECONSEQUENCEUI_H_INCLUDED
