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

class ScriptedConsequence : public RuleConsequence
{
public:
	ScriptedConsequence();
	virtual ~ScriptedConsequence();

	CodeDocument codeDocument;

	virtual void evaluate() override;

	RuleConsequenceUI * createUI() override;
};

#endif  // SCRIPTEDCONSEQUENCE_H_INCLUDED
