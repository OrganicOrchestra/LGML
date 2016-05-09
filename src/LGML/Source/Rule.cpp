/*
  ==============================================================================

    Rule.cpp
    Created: 4 May 2016 5:06:55pm
    Author:  bkupe

  ==============================================================================
*/

#include "Rule.h"

Rule::Rule(const String &_name) :
	ControllableContainer(_name)
{
	nameParam = addStringParameter("Name", "Name of tue rule", _name);
}

Rule::~Rule()
{
}

void Rule::onContainerParameterChanged(Parameter * p)
{
	if (p == nameParam)
	{
		setNiceName(nameParam->stringValue());
	}
}
