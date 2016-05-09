/*
  ==============================================================================

    RuleEditor.cpp
    Created: 9 May 2016 3:02:21pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleEditor.h"

juce_ImplementSingleton(RuleEditor)

RuleEditor::RuleEditor() :
	currentRule(nullptr)

{
}

RuleEditor::~RuleEditor()
{
}

void RuleEditor::setCurrentRule(Rule * r)
{
	if (currentRule == r) return;

	if (currentRule != nullptr)
	{
		//
	}

	currentRule = r;

	if (currentRule != nullptr)
	{
		//
	}
}

  void RuleEditor::editRule(Rule * r)
  {
	  setCurrentRule(r);
  }
