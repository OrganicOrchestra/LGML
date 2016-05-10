/*
  ==============================================================================

    RuleEditor.cpp
    Created: 9 May 2016 3:02:21pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleEditor.h"

RuleEditor::RuleEditor(RuleUI * _ruleUI) :
	CustomEditor(_ruleUI),
	rule(_ruleUI->rule)
{

}

RuleEditor::~RuleEditor()
{
}


void RuleEditor::paint(Graphics &)
{

}

void RuleEditor::resized()
{

}

void RuleEditor::addConditionGroupUI(RuleConditionGroup * c)
{
	RuleConditionGroupUI * cui = new RuleConditionGroupUI(c);
	conditionGroupsUI.add(cui);
	addAndMakeVisible(cui);
}

void RuleEditor::addConsequenceUI(RuleConsequence * c)
{
	RuleConsequenceUI * cui = new RuleConsequenceUI(c);
	consequencesUI.add(cui);
	addAndMakeVisible(cui);
}

