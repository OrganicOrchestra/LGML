/*
  ==============================================================================

    FactoryObject.h
    Created: 12 Sep 2017 10:15:49am
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

// compile time check that all subclass has implemented
#define DECLARE_OBJECT_BASE(T)     virtual int dummy_##T() = 0;
#define CHK_OBJECT_BASE(T)  int dummy_check_##T = dummy_##T();ignoreUnused(dummy_check_##T);
#define IMPL_OBJECT_BASE(T)    virtual int dummy_##T() {return 0;};

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






#define DECLARE_OBJ_TYPE_DEFAULTNAME(T,DEFAULTNAME) static const Identifier _objType; \
const Identifier & getTypeId() const override {return _objType;}\
static const Identifier & getClassId() {return _objType;}\
const String & getTypeName() const override {return _objType.toString();}\
T(StringRef name=DEFAULTNAME);


#define DECLARE_OBJ_TYPE(T) DECLARE_OBJ_TYPE_DEFAULTNAME(T,#T)
// use that for static objects that dont need factories
#define IMPL_OBJ_TYPE(T)  const Identifier T::_objType = Identifier( "t_" #T);

