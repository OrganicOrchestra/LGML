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
	jsEditor(&condition->codeDocument),
	reloadBT("Reload Script")
{
	addAndMakeVisible(&jsEditor);
	addAndMakeVisible(&reloadBT);
	reloadBT.addListener(this);
}

ScriptedConditionUI::~ScriptedConditionUI()
{
}

void ScriptedConditionUI::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(2);
	reloadBT.setBounds(r.removeFromBottom(15));
	jsEditor.setBounds(r);
}

void ScriptedConditionUI::buttonClicked(Button * b)
{
	if (b == &reloadBT) scriptedCondition->reloadScript();
}

