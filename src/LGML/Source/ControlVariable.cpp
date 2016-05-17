/*
  ==============================================================================

    ControlVariable.cpp
    Created: 10 May 2016 3:17:16pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControlVariable.h"
#include "DebugHelpers.h"
#include "Controller.h"

ControlVariable::ControlVariable(Controller * c, Parameter * p) :
	controller(c),
	parameter(p)
{
	p->isEditable = false;
}

ControlVariable::~ControlVariable()
{
	variableListeners.call(&ControlVariableListener::variableRemoved, this);
}

void ControlVariable::remove()
{
	NLOG("ControlVariable", "remove");

	variableListeners.call(&ControlVariableListener::askForRemoveVariable, this);
}
