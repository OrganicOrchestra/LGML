/*
  ==============================================================================

    ScriptedConsequence.cpp
    Created: 13 May 2016 6:40:37pm
    Author:  bkupe

  ==============================================================================
*/

#include "ScriptedConsequence.h"
#include "ScriptedConsequenceUI.h"

ScriptedConsequence::ScriptedConsequence() :
	RuleConsequence()
{
}

ScriptedConsequence::~ScriptedConsequence()
{
}

void ScriptedConsequence::evaluate()
{
}

RuleConsequenceUI * ScriptedConsequence::createUI()
{
	return new ScriptedConsequenceUI(this);
}
