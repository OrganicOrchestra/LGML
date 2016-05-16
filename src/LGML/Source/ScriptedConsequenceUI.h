/*
  ==============================================================================

    ScriptedConsequenceUI.h
    Created: 13 May 2016 6:41:05pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SCRIPTEDCONSEQUENCEUI_H_INCLUDED
#define SCRIPTEDCONSEQUENCEUI_H_INCLUDED

#include "RuleConsequenceUI.h"
#include "ScriptedConsequence.h"

class ScriptedConsequenceUI : public RuleConsequenceUI
{
public:
	ScriptedConsequenceUI(ScriptedConsequence * consequence);
	virtual ~ScriptedConsequenceUI();

	ScriptedConsequence * scriptedConsequence;

	CPlusPlusCodeTokeniser codeTokeniser;
	CodeEditorComponent codeEditor;

	void resized() override;

};



#endif  // SCRIPTEDCONSEQUENCEUI_H_INCLUDED
