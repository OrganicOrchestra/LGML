/*
  ==============================================================================

    StringParameter.cpp
    Created: 9 Mar 2016 12:29:30am
    Author:  bkupe

  ==============================================================================
*/

#include "StringParameter.h"
#include "StringParameterUI.h"


StringParameter::StringParameter(const String & niceName, const String & initialValue) :
	Parameter(niceName)
{
	setValue(initialValue);
}

StringParameterUI * StringParameter::getUI()
{
	return new StringParameterUI(this);
}

