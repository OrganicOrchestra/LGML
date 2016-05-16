/*
  ==============================================================================

    ScriptedConditionUI.cpp
    Created: 13 May 2016 6:41:16pm
    Author:  bkupe

  ==============================================================================
*/

#include "ScriptedConditionUI.h"

ScriptedConditionUI::ScriptedConditionUI(ScriptedCondition * condition) :
	RuleConditionUI(condition),
	scriptedCondition(condition),
	codeEditor(condition->codeDocument,&codeTokeniser)
{
	addAndMakeVisible(&codeEditor);
}

ScriptedConditionUI::~ScriptedConditionUI()
{
}

void ScriptedConditionUI::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(2);
	codeEditor.setBounds(r);
}
