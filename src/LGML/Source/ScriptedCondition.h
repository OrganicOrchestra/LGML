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

class ScriptedCondition : public RuleCondition
{
public :
	ScriptedCondition();
	virtual ~ScriptedCondition();

	CodeDocument codeDocument;
};



#endif  // SCRIPTEDCONDITION_H_INCLUDED
