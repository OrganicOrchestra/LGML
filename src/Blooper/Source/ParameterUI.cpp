/*
  ==============================================================================

    ParameterUI.cpp
    Created: 8 Mar 2016 3:48:44pm
    Author:  bkupe

  ==============================================================================
*/

#include "ParameterUI.h"

//==============================================================================
ParameterUI::ParameterUI(Parameter * parameter) :
	parameter(parameter), ControllableUI(parameter)
{
	parameter->addParameterListener(this);

}

ParameterUI::~ParameterUI()
{
}

void ParameterUI::parameterValueChanged(Parameter * p)
{
	//to be overriden
}
