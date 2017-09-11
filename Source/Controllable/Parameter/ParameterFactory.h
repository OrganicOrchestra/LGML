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
//#include "../Controllable/Controllable.h"
class Controllable;
class Parameter;

class ParameterFactory{
public:


  static Parameter* createFromVarObject(var v ,const String & name);
  static var getVarObjectFromControllable(Controllable *c);
  static const String & getIdentifierForInstance(Controllable * );


  template<typename T>
  static String registerType (const String & ID){
    jassert(!getFactory().contains(ID));
    getFactory().set(ID, Entry(createFromVar<T>));
    return ID;
  }

  /////////
  //intern
  static void  logAllTypes();

private:


  typedef Parameter* (*CreatorFunc)(const String &niceName, const String &description , DynamicObject* params);
  template <typename T>
  static Parameter* createFromVar( const String &niceName, const String &description , DynamicObject* params) {
    T * res =  new T(niceName,description);
    if(params){
      res->setFromVarObject(*params);
    }
    return res;
  }



  typedef CreatorFunc Entry;

  static  HashMap< String, Entry > & getFactory(){
    static HashMap< String, Entry > factory;
    return factory;
  }

};





#define REGISTER_PARAM_TYPE(T)  const String T::_paramType = "t_" #T;\
 static const String _type_##T = ParameterFactory::registerType<T>(T::_paramType);


#define REGISTER_PARAM_TYPE_TEMPLATED(T,TT) template<> const String T<TT>::_paramType = #T "_" #TT; \
static const String type_##T_##TT = ParameterFactory::registerType< T<TT> >(T<TT>::_paramType);



