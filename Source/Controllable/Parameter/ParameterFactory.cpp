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

#include "Parameter.h"


static Identifier descIdentifier("_description");



void ParameterFactory::logAllTypes (){
  DBG("Parameter types : ");
  for(auto it = getFactory().begin();it != getFactory().end() ; ++it){
      DBG("\t" + it.getKey());
  }
}

const String & ParameterFactory::getIdentifierForInstance(Controllable * c){
  if(auto cp = c->getParameter()){return cp->getTypeId();}
  jassertfalse;
  return String::empty;
};

Parameter* ParameterFactory::createFromVarObject(var v ,const String & name){
  DynamicObject * ob = v.getDynamicObject();
  String ID =ob->getProperty(Parameter::varTypeIdentifier);
  String desc =ob->getProperty(descIdentifier);
  if (getFactory().contains(ID)){
  return getFactory()[ID](name,desc,v.getDynamicObject());
  }
  else{
    jassertfalse;
    return nullptr;
  }
}

var ParameterFactory::getVarObjectFromControllable(Controllable *c){
  var  res = c->getVarObject();
  res.getDynamicObject()->setProperty(Parameter::varTypeIdentifier, getIdentifierForInstance(c));
  return res;
}



