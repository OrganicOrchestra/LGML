/*
 ==============================================================================

 ControllableFactory.cpp
 Created: 20 Jul 2017 7:31:07pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "ControllableFactory.h"



#include "Trigger.h"
#include "BoolParameter.h"
#include "FloatParameter.h"
#include "IntParameter.h"
#include "EnumParameter.h"
#include "StringParameter.h"
#include "ParameterProxy.h"
static Identifier descIdentifier("_description");


typedef ControllableFactory::IdentifierType IdentifierType;

HashMap< IdentifierType, ControllableFactory::Entry > ControllableFactory::factory;




template<typename T>
void ControllableFactory::registerType (const IdentifierType & ID){
  jassert(!factory.contains(ID));
  factory.set(ID, Entry(createFromVar<T>,checkNget<T>));
  
  DBG(  (int64)&typeid(T) << "," << ID);

}

#define REG(T) registerType<T>("t_" #T)

bool ControllableFactory::registerAllTypes (){
  static bool hasBeenRegistered = false;
  jassert(!hasBeenRegistered);

  REG(Trigger);
  REG(BoolParameter);
  REG(FloatParameter);
  REG(IntParameter);
  REG(EnumParameter);
  REG(StringParameter);
  REG(ParameterProxy);





  hasBeenRegistered = true;
  return true;
}

const IdentifierType ControllableFactory::getIdentifierForInstance(Controllable * c){
  for(auto it = factory.begin();it != factory.end() ; ++it){
    if(std::get<1>(it.getValue())(c)!=nullptr){
      return it.getKey();
    }
  }
  jassertfalse;
  return "Not Found";
};

Controllable* ControllableFactory::createFromVarObject(var v ,const String & name){
  DynamicObject * ob = v.getDynamicObject();
  IdentifierType ID =ob->getProperty(Controllable::varTypeIdentifier);
  String desc =ob->getProperty(descIdentifier);


  return std::get<0>(factory[ID])(name,desc,v.getDynamicObject());
}

var ControllableFactory::getVarObjectFromControllable(Controllable *c){
  var  res = c->getVarObject();
  res.getDynamicObject()->setProperty(Controllable::varTypeIdentifier, getIdentifierForInstance(c));
  return res;
}



