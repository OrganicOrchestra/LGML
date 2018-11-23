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

#if !ENGINE_HEADLESS

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

#include "../ParameterList.h"
#include "ParameterListUI.h"



#define CHKNRETURN(p,classN,UIN)  if(p->isType< classN >()) {return new UIN((classN*)p);}
#define CHKNRETURNSLIDER(p,classN,UIN)      if(auto s = dynamic_cast<classN *>(t)){ \
                                                if(s->hasFiniteBounds()){ \
                                                    return new UIN(s); \
                                                } \
}

//#define REG(cls,meth)
ParameterUI* ParameterUIFactory::createDefaultUI ( ParameterBase* t)
{

    CHKNRETURN (t, BoolParameter, BoolToggleUI)
    CHKNRETURN (t, StringParameter, StringParameterUI)
    CHKNRETURN (t, FloatParameter, FloatSliderUI)
    CHKNRETURN (t, Trigger, TriggerBlinkUI)
    CHKNRETURN (t, IntParameter, IntStepperUI)
    CHKNRETURN (t, EnumParameter, EnumParameterUI)
    CHKNRETURN (t, RangeParameter, RangeParameterUI)
    CHKNRETURN (t, ParameterProxy, ParameterProxyUI)
    CHKNRETURNSLIDER(t,ParameterList<int>, ParameterListUI<int>)
    CHKNRETURNSLIDER(t,ParameterList<floatParamType>, ParameterListUI<floatParamType>)
    
    //jassertfalse;
    return new StringParameterUI(t);



}

#endif
