/*
  ==============================================================================

    ScriptedConsequence.h
    Created: 13 May 2016 6:40:37pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SCRIPTEDCONSEQUENCE_H_INCLUDED
#define SCRIPTEDCONSEQUENCE_H_INCLUDED

#include "RuleConsequence.h"
#include "JsEnvironment.h"

class ScriptedConsequence :
	public RuleConsequence,
	public JsEnvironment
{
public:
	ScriptedConsequence(Rule * r);
	virtual ~ScriptedConsequence();

	CodeDocument codeDocument;

	virtual void run() override;

	// Inherited via JsEnvironment
	virtual void buildLocalEnv() override;

	void reloadScript();

	virtual void referenceAliasChanged(Rule * r, ControlVariableReference *) override;
	virtual void ruleActivationChanged(Rule * r) override;

	virtual var getJSONData() override;
	virtual void loadJSONData(var data) override;
	
	RuleConsequenceUI * createUI() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScriptedConsequence)

};

#endif  // SCRIPTEDCONSEQUENCE_H_INCLUDED
