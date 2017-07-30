/*
 ==============================================================================

 ParameterUI.cpp
 Created: 8 Mar 2016 3:48:44pm
 Author:  bkupe

 ==============================================================================
 */

#include "ParameterUI.h"
#include "DebugHelpers.h"

//==============================================================================
ParameterUI::ParameterUI(Parameter * parameter) :
parameter(parameter),
ControllableUI(parameter),
showLabel(true),
showValue(true),
customTextDisplayed(String::empty)
{
  parameter->addAsyncCoalescedListener(this);
  parameter->addParameterListener(this);
  

}

ParameterUI::~ParameterUI()
{
  if(parameter.get()){
    parameter->removeParameterListener(this);
    parameter->removeAsyncParameterListener(this);
  }
}


void ParameterUI::setCustomText(const String text){
  customTextDisplayed = text;
  repaint();
}



bool ParameterUI::shouldBailOut(){
  bool bailOut= parameter.get()==nullptr;
  // we want a clean deletion no?
  if(bailOut){
    // TODO : changing vst preset sometimes hit that
    NLOG("ParameterUI", "old component still displayed");
    //jassertfalse;
  }
  return bailOut;

}
