/*
  ==============================================================================

    FactoryObject.h
    Created: 12 Sep 2017 10:15:49am
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

class FactoryObject {
public:

  virtual ~FactoryObject(){};
  virtual const Identifier & getTypeId() const = 0;
  virtual const String & getTypeName() const = 0;
  virtual void configureFromObject(DynamicObject *) = 0;
  virtual DynamicObject * getObject()=0;
  template<typename T>
  bool isType(){return getTypeId() == T::_objType;}
  
};

#define DECLARE_OBJ_TYPE(T) static const Identifier _objType; \
const Identifier & getTypeId() const override {return _objType;}\
static const Identifier & getClassId() {return _objType;}\
const String & getTypeName() const override {return _objType.toString();}\
T(StringRef name=#T);



// use that for static objects that dont need factories
#define IMPL_OBJ_TYPE(T)  const Identifier T::_objType = Identifier( "t_" #T);\
