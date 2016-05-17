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
#include "ControlVariableReference.h"
#include "Rule.h"

class RuleConsequenceUI;

class RuleConsequence: public Parameter::Listener,
	public ControlVariableReference::ControlVariableReferenceListener,
	public Rule::RuleListener
{
public:
	RuleConsequence(Rule * r);
	virtual ~RuleConsequence();

	Rule * rule;


	Array<ControlVariableReference *> references;
	virtual void setReferences(OwnedArray<ControlVariableReference> * _ref);


	virtual void run();


	// Inherited via Listener
	virtual void parameterValueChanged(Parameter * p) override;

	virtual void currentReferenceChanged(ControlVariableReference *, ControlVariable * oldVariable, ControlVariable * newVariable) override;
	virtual void referenceAliasChanged(ControlVariableReference *) override {};

	virtual void ruleActivationChanged(Rule * r) override;

	void clearListeners();

	virtual RuleConsequenceUI * createUI();
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleConsequence)
};




#endif  // RULECONSEQUENCE_H_INCLUDED
