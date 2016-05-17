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
#include "Rule.h"

class RuleConsequenceUI;

class RuleConsequence:
	public RuleListener
{
public:
	RuleConsequence(Rule * r);
	virtual ~RuleConsequence();

	Rule * rule;

	virtual void run();

	// Inherited via Listener
	virtual void referenceValueUpdate(Rule * r, ControlVariableReference * cvr) override;
	virtual void ruleActivationChanged(Rule * r) override;

	virtual var getJSONData();
	virtual void loadJSONData(var data);

	void remove();

	ListenerList<RuleConsequenceListener> consequenceListeners;
	void addConsequenceListener(RuleConsequenceListener* newListener) { consequenceListeners.add(newListener); }
	void removeConsequenceListener(RuleConsequenceListener* listener) { consequenceListeners.remove(listener); }


	virtual RuleConsequenceUI * createUI();
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RuleConsequence)
};




#endif  // RULECONSEQUENCE_H_INCLUDED
