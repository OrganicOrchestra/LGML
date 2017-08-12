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


#include "Controllable.h"
#include "ControllableContainer.h"
#include "JsHelpers.h"
#include "StringUtil.hpp"


#include "ParameterFactory.h"



const Identifier Controllable::varTypeIdentifier("_tName");

Controllable::Controllable(const Type &type, const String & niceName, const String &description, bool enabled) :
type(type),
description(description),
parentContainer(nullptr),
hasCustomShortName(false),
isControllableExposed(true),
hideInEditor(false),
shouldSaveObject(false),
isSavable(true),
enabled(enabled),
isUserDefined(false)
{
  setEnabled(enabled);
  setNiceName(niceName);
}

Controllable::~Controllable() {
  Controllable::masterReference.clear();
  listeners.call(&Controllable::Listener::controllableRemoved, this);

}

void Controllable::setNiceName(const String & _niceName) {
  if (niceName == _niceName) return;

  this->niceName = _niceName;
  if (!hasCustomShortName) setAutoShortName();
  else listeners.call(&Listener::controllableNameChanged, this);
}

void Controllable::setCustomShortName(const String & _shortName)
{
  this->shortName = _shortName;
  hasCustomShortName = true;
  updateControlAddress();
  listeners.call(&Listener::controllableNameChanged, this);
}

void Controllable::setAutoShortName() {
  hasCustomShortName = false;
  shortName = StringUtil::toShortName(niceName);
  updateControlAddress();
  listeners.call(&Listener::controllableNameChanged, this);
}

void Controllable::setEnabled(bool value, bool silentSet, bool force)
{
  if (!force && value == enabled) return;

  enabled = value;
  if (!silentSet) listeners.call(&Listener::controllableStateChanged, this);
}

void Controllable::setParentContainer(ControllableContainer * container)
{
  this->parentContainer = container;
  updateControlAddress();
}

void Controllable::updateControlAddress()
{
  this->controlAddress = getControlAddress();
  listeners.call(&Listener::controllableControlAddressChanged, this);
}

String Controllable::getControlAddress(ControllableContainer * relativeTo)
{
  // we may need empty parentContainer in unit tests
#if LGML_UNIT_TESTS
  return (parentContainer?parentContainer->getControlAddress(relativeTo):"") + "/"+shortName;
#else
  return parentContainer->getControlAddress(relativeTo) + "/"+shortName;
#endif
}

bool Controllable::isNumeric()
{
  return type == Type::FLOAT || type == Type::INT ;
}

DynamicObject * Controllable::createDynamicObject()
{
  DynamicObject* dObject = new DynamicObject();
  dObject->setProperty(jsPtrIdentifier, (int64)this);
//  dObject->setProperty(jsVarObjectIdentifier, getVarObject());
  dObject->setMethod(jsGetIdentifier, Controllable::getVarStateFromScript);
  return dObject;
}


var Controllable::getVarStateFromScript(const juce::var::NativeFunctionArgs & a)
{
  // TODO handle with weak references
  Parameter * c = getObjectPtrFromJS<Parameter>(a);
  if(c == nullptr  ) return var();
  //  WeakReference<Parameter> wc = c;
  //  if(!wc.get()) return var();
  return c->getVarState();

}
//STATIC


var Controllable::setControllableValueFromJS(const juce::var::NativeFunctionArgs& a) {

  Controllable * c = getObjectPtrFromJS<Controllable>(a);
  //	bool success = false;

  if (c != nullptr)
  {

    //		success = true;

    if (a.numArguments == 0 && c->type == Controllable::Type::TRIGGER) {
      ((Trigger *)c)->trigger();
    }

    else {
      var value = a.arguments[0];
      bool valueIsABool = value.isBool();
      bool valueIsANumber = value.isDouble() || value.isInt() || value.isInt64();

      switch (c->type)
      {
        case Controllable::Type::TRIGGER:
          if (valueIsABool)
          {
            if ((bool)value) ((Trigger *)c)->trigger();
          }
          else if (valueIsANumber)
          {
            if ((float)value >= 1) ((Trigger *)c)->trigger();
          }

          break;

        case Controllable::Type::BOOL:

          if (valueIsABool) ((BoolParameter *)c)->setValue((bool)value);
          else if (valueIsANumber) ((BoolParameter *)c)->setValue((float)value > .5);
          break;

        case Controllable::Type::FLOAT:
          if (valueIsABool || valueIsANumber) ((FloatParameter *)c)->setValue(value);
          break;
        case Controllable::Type::INT:
          if (valueIsABool || valueIsANumber) ((IntParameter *)c)->setValue(value);
          break;


        case Controllable::Type::STRING:
          if (value.isString()) ((StringParameter *)c)->setValue(value);
          break;
          
        default:
          //				success = false;
          break;
          
      }
    }
  }
  
  
  return var();
}


const Identifier  Controllable::getTypeIdentifier(){
  return ParameterFactory::getIdentifierForInstance(this);
}


Parameter* Controllable::getParameter(){
  // static cast as no pure controllable
  // can be called on nullptr
  return static_cast<Parameter*>(this);
}

bool Controllable::isMappable(){
  if(auto p = getParameter()){
  return p->isEditable && type!=Controllable::Type::STRING;
  }
  return false;
}

bool Controllable::isChildOf(ControllableContainer * p){
  auto i = parentContainer;
  while(i){
    if(i==p){
      return true;
    }
    i = i->parentContainer;
  }
  return false;

}
