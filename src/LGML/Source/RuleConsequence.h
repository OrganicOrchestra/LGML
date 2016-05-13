/*
  ==============================================================================

    RuleConsequence.h
    Created: 9 May 2016 2:55:21pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULECONSEQUENCE_H_INCLUDED
#define RULECONSEQUENCE_H_INCLUDED

#include "JuceHeader.h"

class RuleConsequenceUI;

class RuleConsequence
{
public:
	RuleConsequence();
	virtual ~RuleConsequence();

	virtual void evaluate();

	virtual RuleConsequenceUI * createUI();
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleConsequence)
};




#endif  // RULECONSEQUENCE_H_INCLUDED
