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


#include "ParameterUIFactory.h"
#include "../BoolParameter.h"
#include "BoolToggleUI.h"

#include "../NumericParameter.h"
#include "SliderUI.h"
#include "StepperUI.h"

#include "../Trigger.h"
#include "TriggerBlinkUI.h"

#include "../EnumParameter.h"
#include "EnumParameterUI.h"

#include "../StringParameter.h"
#include "StringParameterUI.h"


#include "../ParameterProxy.h"
#include "ParameterProxyUI.h"

#include "../RangeParameter.h"
#include "RangeParameterUI.h"



#define CHKNRETURN(p,classN,UIN)  if(p->getTypeId()==classN::_objType) {return new UIN((classN*)p);}

//#define REG(cls,meth)
ParameterUI * ParameterUIFactory::createDefaultUI(Parameter * t) {

  CHKNRETURN(t,BoolParameter,BoolToggleUI)
  CHKNRETURN(t,StringParameter,StringParameterUI)
  CHKNRETURN(t,FloatParameter,FloatSliderUI)
  CHKNRETURN(t,Trigger,TriggerBlinkUI)
  CHKNRETURN(t,IntParameter,IntStepperUI)
  CHKNRETURN(t,EnumParameter,EnumParameterUI)
  CHKNRETURN(t,RangeParameter,RangeParameterUI)
  CHKNRETURN(t,ParameterProxy,ParameterProxyUI)

  jassertfalse;
  return nullptr;


}
