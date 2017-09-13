/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include "ParameterUI.h"
#include "../../../Utils/DebugHelpers.h"

//==============================================================================
ParameterUI::ParameterUI(Parameter * _parameter) :
parameter(_parameter),
ControllableUI(_parameter),
showLabel(true),
showValue(true),
customTextDisplayed(String::empty)
{
  if(parameter.get()){
  parameter->addAsyncCoalescedListener(this);
  parameter->addParameterListener(this);
  }
  else{
    jassertfalse;
  }
  

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
    NLOG("ParameterUI", "!!! old component still displayed");
    //jassertfalse;
  }
  return bailOut;

}
