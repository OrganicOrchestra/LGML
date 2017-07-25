/*
  ==============================================================================

    ParameterUIFactory.cpp
    Created: 24 Jul 2017 8:23:35pm
    Author:  Martin Hermant

  ==============================================================================
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
     default:
       jassertfalse;
       return nullptr;
   }

}
