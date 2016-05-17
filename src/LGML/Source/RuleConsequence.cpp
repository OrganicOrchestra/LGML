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


void RuleConsequence::setReferences(OwnedArray<ControlVariableReference> * _ref)
{
	clearListeners();

	references.clear();

	for (auto & r : *_ref)
	{
		references.add(r);

		r->addReferenceListener(this);

	}
}

void RuleConsequence::run()
{
	DBG("Run");
}


void RuleConsequence::parameterValueChanged(Parameter *)
{
	DBG("Rule Consequence param value changed");
	run();
}

void RuleConsequence::currentReferenceChanged(ControlVariableReference *, ControlVariable * oldVariable, ControlVariable * newVariable)
{
	if (oldVariable != nullptr) oldVariable->parameter->removeParameterListener(this);
	if (newVariable != nullptr) newVariable->parameter->addParameterListener(this);
}


void RuleConsequence::ruleActivationChanged(Rule * r)
{
	DBG("Rule Consequence Rule Activation changed ! " << String(r->isActive()));
}

void RuleConsequence::clearListeners()
{
	for (auto &r : references)
	{
		r->removeReferenceListener(this);

		if (r->referenceParam != nullptr) r->referenceParam->removeParameterListener(this);
	}
}


RuleConsequenceUI * RuleConsequence::createUI()
{
	return new RuleConsequenceUI(this);
}
