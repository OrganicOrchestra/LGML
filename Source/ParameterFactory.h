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


#pragma once

#include "JuceHeader.h"
#include "Controllable.h"


class ParameterFactory{
public:
  
  typedef String IdentifierType;


  static Parameter* createFromVarObject(var v ,const String & name);
  static var getVarObjectFromControllable(Controllable *c);
  static const IdentifierType getIdentifierForInstance(Controllable * );


  /////////
  //intern
  static bool  registerAllTypes();

  private:
  template<typename T>
  static void registerType (const IdentifierType & ID);

  


  typedef Parameter* (*CreatorFunc)(const String &niceName, const String &description , DynamicObject* params);
  typedef Parameter* (*CheckFunction)(Controllable* c);
  typedef std::tuple<CreatorFunc,CheckFunction> Entry;


  static HashMap< IdentifierType, Entry > factory;
  


  template <typename T>
  static Parameter* createFromVar( const String &niceName, const String &description , DynamicObject* params) {
    T * res =  new T(niceName,description);
    if(params){
    res->setFromVarObject(*params);
    }
    return res;
  }
  template <typename T>
  static Parameter* checkNget(Controllable *c) {
    return dynamic_cast<T*>(c);
  }
  
  
};

