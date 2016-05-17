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
	ScriptedCondition();
	virtual ~ScriptedCondition();

	
	CodeDocument codeDocument;

	virtual void setReferences(OwnedArray<ControlVariableReference> *_ref) override;

	virtual bool evaluateInternal();

	// Inherited via JsEnvironment
	virtual void buildLocalEnv() override;

	void reloadScript();

	virtual void currentReferenceChanged(ControlVariableReference *, ControlVariable * oldVariable, ControlVariable * newVariable) override;
	virtual void referenceAliasChanged(ControlVariableReference *) override;




};



#endif  // SCRIPTEDCONDITION_H_INCLUDED
