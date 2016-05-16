/*
  ==============================================================================

    ControlVariable.cpp
    Created: 10 May 2016 3:17:16pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControlVariable.h"
#include "DebugHelpers.h"

ControlVariable::ControlVariable(Parameter * p) :
	parameter(p)
{
	p->isEditable = false;
}

ControlVariable::~ControlVariable()
{
}

void ControlVariable::remove()
{
	NLOG("ControlVariable", "remove");

	variableListeners.call(&VariableListener::askForRemoveVariable, this);
}
