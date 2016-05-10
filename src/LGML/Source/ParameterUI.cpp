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
parameter(parameter),
ControllableUI(parameter),
showLabel(true),
showValue(true)
{
    parameter->addParameterListener(this);

}

ParameterUI::~ParameterUI()
{
    if(parameter.get())parameter->removeParameterListener(this);
}



bool ParameterUI::shouldBailOut(){
    bool bailOut= parameter.get()==nullptr;
    // we want a clean deletion no?
    // comment this to continue safely to see what happen next
    jassert(!bailOut);
    return bailOut;

}
