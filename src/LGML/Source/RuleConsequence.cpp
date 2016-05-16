/*
  ==============================================================================

    RuleConsequence.cpp
    Created: 9 May 2016 2:55:21pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleConsequence.h"
#include "RuleConsequenceUI.h"

RuleConsequence::RuleConsequence()
{
}

RuleConsequence::~RuleConsequence()
{
}

void RuleConsequence::evaluate()
{
}

RuleConsequenceUI * RuleConsequence::createUI()
{
	return new RuleConsequenceUI(this);
}
