/*
 ==============================================================================

 ParameterFactory.h
 Created: 20 Jul 2017 7:31:07pm
 Author:  Martin Hermant

 ==============================================================================
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

