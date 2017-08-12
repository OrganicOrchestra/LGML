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
#include "BoolParameter.h"
#include "BoolToggleUI.h"

#include "FloatSliderUI.h"
#include "FloatParameter.h"

#include "IntParameter.h"
//#include "IntSliderUI.h"
#include "IntStepperUI.h"

#include "Trigger.h"
#include "TriggerBlinkUI.h"

#include "EnumParameter.h"
#include "EnumParameterUI.h"

#include "StringParameter.h"
#include "StringParameterUI.h"


#include "ParameterProxy.h"
#include "ParameterProxyUI.h"

#include "RangeParameter.h"
#include "RangeParameterUI.h"





//#define REG(cls,meth)
ParameterUI * ParameterUIFactory::createDefaultUI(Parameter * targetControllable) {
   auto classId = targetControllable->type;
   switch(classId){
     case Controllable::BOOL:
       return new BoolToggleUI(dynamic_cast<BoolParameter *>(targetControllable));
     case Controllable::STRING:
       return  new StringParameterUI(dynamic_cast<StringParameter *>(targetControllable));
     case Controllable::FLOAT:
       return new FloatSliderUI(dynamic_cast<FloatParameter *>(targetControllable));
     case Controllable::TRIGGER:
       return new TriggerBlinkUI(dynamic_cast<Trigger *>(targetControllable));
     case Controllable::INT:
       return new IntStepperUI(dynamic_cast<IntParameter *>(targetControllable));
     case Controllable::ENUM:
       return new EnumParameterUI(dynamic_cast<EnumParameter *>(targetControllable));
     case Controllable::PROXY:
       return new ParameterProxyUI(dynamic_cast<ParameterProxy *>(targetControllable));
     case Controllable::RANGE:
       return new RangeParameterUI(dynamic_cast<RangeParameter *>(targetControllable));
     default:
       jassertfalse;
       return nullptr;
   }

}
