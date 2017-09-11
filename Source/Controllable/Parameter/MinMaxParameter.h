/*
 ==============================================================================

 MinMaxParameter.h
 Created: 5 Sep 2017 1:57:42pm
 Author:  Martin Hermant

 ==============================================================================
 */

#pragma once

#include "Parameter.h"

namespace{
  static const Identifier minValueIdentifier("_min");
  static const Identifier maxValueIdentifier("_max");
}
class MinMaxParameter : public Parameter{
public:
  MinMaxParameter(Controllable::Type type, const String &niceName, const String &description,
                  const var &initialValue=0,const var min=var::undefined(),const var max=var::undefined(),
                  bool enabled=true):
  Parameter(type,niceName,description,initialValue,enabled),
  minimumValue(min),
  maximumValue(max){

  }
  virtual ~MinMaxParameter(){};

  var minimumValue;
  var maximumValue;


  var getVarObject() override {
    auto res = Parameter::getVarObject();
    res.getDynamicObject()->setProperty(minValueIdentifier, minimumValue);
    res.getDynamicObject()->setProperty(maxValueIdentifier, maximumValue);
    return res;
  }

  void setFromVarObject(DynamicObject & ob) override{
    Parameter::setFromVarObject(ob);
    if(ob.hasProperty(minValueIdentifier)){minimumValue = ob.getProperty(minValueIdentifier);}
    if(ob.hasProperty(maxValueIdentifier)){maximumValue = ob.getProperty(maxValueIdentifier);}
  }

  void setMinMax(var min, var max){
    minimumValue = min;
    maximumValue = max;
    listeners.call(&Listener::parameterRangeChanged,this);
    var arr;arr.append(minimumValue);arr.append(maximumValue);
    queuedNotifier.addMessage(new ParamWithValue(this,arr,true));
  }
  
};
