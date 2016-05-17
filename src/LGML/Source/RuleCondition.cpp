/*
  ==============================================================================

    RuleCondition.cpp
    Created: 9 May 2016 2:55:10pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleCondition.h"
#include "DebugHelpers.h"


RuleCondition::RuleCondition(RuleConditionGroup * _parent) :
	parent(_parent),
	isActive(false)
{
}

RuleCondition::~RuleCondition()
{
	clearListeners();
}

void RuleCondition::setActive(bool value)
{
	if (isActive == value) return;
	isActive = value;
	DBG("Rule Condition setActive : " + String(isActive));
	conditionListeners.call(&RuleConditionListener::conditionActivationChanged, this);
}


void RuleCondition::setReferences(OwnedArray<ControlVariableReference> * _ref)
{
	clearListeners();

	references.clear();

	for (auto & r : *_ref)
	{
		references.add(r);

		r->addReferenceListener(this); 
		
	}
}

void RuleCondition::evaluate()
{
	
	bool result = evaluateInternal();
	setActive(result);
}

bool RuleCondition::evaluateInternal()
{
	return false;
}

void RuleCondition::remove()
{
	conditionListeners.call(&RuleConditionListener::askForRemoveCondition, this);
}

void RuleCondition::parameterValueChanged(Parameter *)
{
	evaluate();
}

void RuleCondition::currentReferenceChanged(ControlVariableReference * , ControlVariable * oldVariable, ControlVariable * newVariable)
{
	if (oldVariable != nullptr) oldVariable->parameter->removeParameterListener(this);
	if (newVariable != nullptr) newVariable->parameter->addParameterListener(this);
}


void RuleCondition::clearListeners()
{
	for (auto &r : references)
	{
		r->removeReferenceListener(this);

		if (r->referenceParam != nullptr) r->referenceParam->removeParameterListener(this);
	}
}
