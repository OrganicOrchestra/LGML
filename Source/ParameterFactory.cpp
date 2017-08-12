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


#include "ParameterFactory.h"



#include "Trigger.h"
#include "BoolParameter.h"
#include "FloatParameter.h"
#include "IntParameter.h"
#include "EnumParameter.h"
#include "StringParameter.h"
#include "ParameterProxy.h"
#include "RangeParameter.h"
#include "Point2DParameter.h"

static Identifier descIdentifier("_description");


typedef ParameterFactory::IdentifierType IdentifierType;

HashMap< IdentifierType, ParameterFactory::Entry > ParameterFactory::factory;


template<typename T>
void ParameterFactory::registerType (const IdentifierType & ID){
  jassert(!factory.contains(ID));
  factory.set(ID, Entry(createFromVar<T>,checkNget<T>));
}

#define REG(T) registerType<T>("t_" #T)

bool ParameterFactory::registerAllTypes (){
  static bool hasBeenRegistered = false;
  jassert(!hasBeenRegistered);

  REG(Trigger);
  REG(BoolParameter);
  REG(FloatParameter);
  REG(IntParameter);
  REG(EnumParameter);
  REG(StringParameter);
  REG(ParameterProxy);
  REG(RangeParameter);
  REG(Point2DParameter<int>);
  REG(Point2DParameter<float>);

  hasBeenRegistered = true;
  return true;
}

const IdentifierType ParameterFactory::getIdentifierForInstance(Controllable * c){
  for(auto it = factory.begin();it != factory.end() ; ++it){
    if(std::get<1>(it.getValue())(c)!=nullptr){
      return it.getKey();
    }
  }
  jassertfalse;
  return "Not Found";
};

Parameter* ParameterFactory::createFromVarObject(var v ,const String & name){
  DynamicObject * ob = v.getDynamicObject();
  IdentifierType ID =ob->getProperty(Controllable::varTypeIdentifier);
  String desc =ob->getProperty(descIdentifier);
  return std::get<0>(factory[ID])(name,desc,v.getDynamicObject());
}

var ParameterFactory::getVarObjectFromControllable(Controllable *c){
  var  res = c->getVarObject();
  res.getDynamicObject()->setProperty(Controllable::varTypeIdentifier, getIdentifierForInstance(c));
  return res;
}



