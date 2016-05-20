/*
  ==============================================================================

    RuleConsequence.cpp
    Created: 9 May 2016 2:55:21pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleConsequence.h"
#include "RuleConsequenceUI.h"

RuleConsequence::RuleConsequence(Rule * r) :
	rule(r)
{
	rule->addRuleListener(this);
}

RuleConsequence::~RuleConsequence()
{
	rule->removeRuleListener(this);
}

void RuleConsequence::run()
{
	//DBG("Run");
}

void RuleConsequence::referenceValueUpdate(Rule *, ControlVariableReference *)
{
	run();
}

void RuleConsequence::ruleActivationChanged(Rule * r)
{
	DBG("Rule Consequence Rule Activation changed ! " << String(r->isActive()));
}


var RuleConsequence::getJSONData()
{
	return var();
}

void RuleConsequence::loadJSONData(var data)
{
}

void RuleConsequence::remove()
{
	consequenceListeners.call(&RuleConsequenceListener::askForRemoveConsequence, this);
}

RuleConsequenceUI * RuleConsequence::createUI()
{
	return new RuleConsequenceUI(this);
}
