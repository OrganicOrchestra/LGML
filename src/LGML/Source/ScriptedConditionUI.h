/*
  ==============================================================================

    ScriptedConditionUI.h
    Created: 13 May 2016 6:41:16pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SCRIPTEDCONDITIONUI_H_INCLUDED
#define SCRIPTEDCONDITIONUI_H_INCLUDED

#include "ScriptedCondition.h"
#include "RuleConditionUI.h"

class ScriptedConditionUI : public RuleConditionUI
{
public :
	ScriptedConditionUI(ScriptedCondition * condition);
	virtual ~ScriptedConditionUI();

	ScriptedCondition * scriptedCondition;

	CPlusPlusCodeTokeniser codeTokeniser;
	CodeEditorComponent codeEditor;
	
	void resized() override;
};



#endif  // SCRIPTEDCONDITIONUI_H_INCLUDED
