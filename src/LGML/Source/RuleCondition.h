/*
  ==============================================================================

    RuleCondition.h
    Created: 9 May 2016 2:55:10pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef RULECONDITION_H_INCLUDED
#define RULECONDITION_H_INCLUDED

#include "JuceHeader.h"
#include "ControlVariableReference.h"

class RuleConditionGroup;

class RuleCondition : public Parameter::Listener, public ControlVariableReference::ControlVariableReferenceListener
{
public:
	RuleCondition(RuleConditionGroup * parent);
	virtual ~RuleCondition();

	RuleConditionGroup * parent;

	bool isActive;
	void setActive(bool value);

	Array<ControlVariableReference *> references;
	virtual void setReferences(OwnedArray<ControlVariableReference> * _ref);

	virtual void evaluate();
	virtual bool evaluateInternal();

	// Inherited via Listener
	virtual void parameterValueChanged(Parameter * p) override;

	virtual void currentReferenceChanged(ControlVariableReference *, ControlVariable * oldVariable, ControlVariable * newVariable) override;
	virtual void referenceAliasChanged(ControlVariableReference *) override {};

	void clearListeners();

	void remove();

	class  RuleConditionListener
	{
	public:
		virtual ~RuleConditionListener() {}

		virtual void askForRemoveCondition(RuleCondition *) {};

		virtual void conditionActivationChanged(RuleCondition *) {}

	};

	ListenerList<RuleConditionListener> conditionListeners;
	void addConditionListener(RuleConditionListener* newListener) { conditionListeners.add(newListener); }
	void removeConditionListener(RuleConditionListener* listener) { conditionListeners.remove(listener); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleCondition)

		
};


#endif  // RULECONDITION_H_INCLUDED
