/*
 ==============================================================================

 Parameter.cpp
 Created: 8 Mar 2016 1:08:19pm
 Author:  bkupe

 ==============================================================================
 */

#include "Parameter.h"
#include "JsHelpers.h"


Parameter::Parameter(const Type &type, const String &niceName, const String &description, var initialValue, var minValue = var(), var maxValue = var(), bool enabled) :
Controllable(type, niceName, description, enabled),
isEditable(true),
isPresettable(true),
isOverriden(false),
queuedNotifier(100),
hasCommitedValue(false),
isCommitableParameter(false)
{
  minimumValue = minValue;
  maximumValue = maxValue;
  defaultValue = initialValue;

  resetValue(true);
}
void Parameter::setFromVarObject(DynamicObject & ob){
if(ob.hasProperty("maximumValue")){minimumValue = ob.getProperty("maximumValue");}
if(ob.hasProperty("minimumValue")){minimumValue = ob.getProperty("minimumValue");}
if(ob.hasProperty("initialValue")){defaultValue = ob.getProperty("initialValue");}
if(ob.hasProperty("value")){setValue(ob.getProperty("value"));}
}
void Parameter::resetValue(bool silentSet)
{
  isOverriden = false;
  setValue(defaultValue, silentSet,true);
}

void Parameter::setValue(var _value, bool silentSet, bool force,bool defferIt)
{
  if(isCommitableParameter && !force){
    commitValue(_value);
  }
  else{
    tryToSetValue(_value,silentSet,force,defferIt);
  }
}
void Parameter::tryToSetValue(var _value, bool silentSet , bool force ,bool defferIt){
  if (!force && checkValueIsTheSame(_value, value)) return;
  lastValue = var(value);
  setValueInternal(_value);
  if(_value != defaultValue) isOverriden = true;
  if (!silentSet) notifyValueChanged(defferIt);

}
void Parameter::setRange(var min, var max){
  minimumValue = min;
  maximumValue = max;
  listeners.call(&Listener::parameterRangeChanged,this);
  var arr;arr.append(minimumValue);arr.append(maximumValue);
  queuedNotifier.addMessage(new ParamWithValue(this,arr));
}

void Parameter::commitValue(var _value){
  hasCommitedValue = value!=_value;
  commitedValue  =_value;

}

void Parameter::pushValue(bool defered,bool force){
  if(!hasCommitedValue && !force)return;
  tryToSetValue(commitedValue,false,true,defered);
  hasCommitedValue = false;
}


void Parameter::setValueInternal(var & _value) //to override by child classes
{

  value = _value;
#ifdef JUCE_DEBUG
  checkVarIsConsistentWithType();
#endif
}

bool Parameter::checkValueIsTheSame(var newValue, var oldValue)
{
  return newValue.hasSameTypeAs(oldValue) && (newValue == oldValue);
}

void Parameter::checkVarIsConsistentWithType(){
  if      (type == Type::STRING && !value.isString()) { value = value.toString();}
  else if (type == Type::INT && !value.isInt())       { value = int(value);}
  else if (type == Type::BOOL && !value.isBool())     { value = bool(value);}
  else if (type == Type::FLOAT && !value.isDouble())  { value = double(value);}
}

void Parameter::setNormalizedValue(const float & normalizedValue, bool silentSet, bool force)
{
  setValue(jmap<float>(normalizedValue, (float)minimumValue, (float)maximumValue), silentSet, force);
}

float Parameter::getNormalizedValue() {
  if(minimumValue==maximumValue){
    return 0.0;
  }
  else
    return jmap<float>((float)value, (float)minimumValue, (float)maximumValue, 0.f, 1.f);
}

void Parameter::notifyValueChanged(bool defferIt) {
  if(defferIt)
    triggerAsyncUpdate();
  else
    listeners.call(&Listener::parameterValueChanged, this);

  queuedNotifier.addMessage(new ParamWithValue(this,value));
}


//JS Helper


DynamicObject * Parameter::createDynamicObject()
{
  DynamicObject * dObject = Controllable::createDynamicObject();


  static const Identifier _jsSetIdentifier("set");
  dObject->setMethod(_jsSetIdentifier, setControllableValueFromJS);


  return dObject;
}
var Parameter::getVarObject(){
    var res = new DynamicObject();
    res.getDynamicObject()->setProperty(varTypeIdentifier, getTypeIdentifier().toString());
    return res;
}
var Parameter::getVarState(){
  return value;
}



void Parameter::handleAsyncUpdate(){
  listeners.call(&Listener::parameterValueChanged, this);
};




