/*
  ==============================================================================

    ScriptedCondition.h
    Created: 13 May 2016 6:40:50pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SCRIPTEDCONDITION_H_INCLUDED
#define SCRIPTEDCONDITION_H_INCLUDED

#include "RuleCondition.h"
#include "JsEnvironment.h"

class ScriptedCondition : 
	public RuleCondition, 
	public JsEnvironment
{
public :
	ScriptedCondition(Rule * r);
	virtual ~ScriptedCondition();

	
	CodeDocument codeDocument;

	virtual bool evaluateInternal();

	// Inherited via JsEnvironment
	virtual void buildLocalEnv() override;
	void reloadScript();

	virtual var getJSONData() override;
	virtual void loadJSONData(var data) override;

	virtual void referenceAliasChanged(Rule * r, ControlVariableReference * cvr) override;

};
	



#endif  // SCRIPTEDCONDITION_H_INCLUDED
