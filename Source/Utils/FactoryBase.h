/*
  ==============================================================================

    FactoryBase.h
    Created: 12 Sep 2017 10:06:51am
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

static const Identifier factoryTypeIdentifier ("_tName");

template <typename CLASSNAME>
class FactoryBase{
public:



  static CLASSNAME* createFromObject(const String & name,DynamicObject * ob ) {
    if (ob){
    Identifier ID (ob->getProperty(factoryTypeIdentifier));
    return createFromTypeID(ID,name,ob);
    }

      jassertfalse;
    return nullptr;


  }

  static CLASSNAME* createFromTypeID(const Identifier & ID,const String & name=String::empty,DynamicObject * ob=nullptr){
    String className = ID.toString();
    String targetName = name.isEmpty()?typeToNiceName(className):name;
    if (getFactory().contains(className)){
      return getFactory()[className](targetName,ob);
    }
    else{
      jassertfalse;
      return nullptr;
    }

  }
  static String typeToNiceName(const String & t){
    if (t.length()>2 && t[0]=='t' && t[1]=='_'){
    return t.substring(2);
    }
    jassertfalse;
    return t;

  }
  static String niceToTypeName(const  String & t){
    if (t.length()<2 || (t[0]!='t' && t[1]!='_')){
      return "t_" + t;
    }
    jassertfalse;
    return t;

  }

  static DynamicObject *  getTypedObjectFromInstance(CLASSNAME *c){
    auto  res = c->getObject();
    res->setProperty(factoryTypeIdentifier, getTypeNameForInstance(c));
    return res;
  }
  
  static const Identifier & getTypeForInstance(CLASSNAME * i ){
    return i->getTypeId();
  };
  static const String & getTypeNameForInstance(CLASSNAME * i ){
    return i->getTypeId().toString();
  };


  template<typename T>
  static String registerType (const String & ID){
    jassert(!getFactory().contains(ID));
    getFactory().set(ID, Entry(createFromObject<T>));
    return ID;
  }

  /////////
  //intern
  static void  logAllTypes(){
    DBG("Factory types (" << typeid(CLASSNAME).name() << ") :");
    auto a = getRegisteredTypes();
    for(auto e:a){
      DBG("\t" + e);
    }
  }

 static Array<String> getRegisteredTypes() {
    Array<String> res;
    for(auto it = getFactory().begin();it != getFactory().end() ; ++it){
      res.add(it.getKey());
    }
    return res;
  }

private:


  typedef CLASSNAME* (*CreatorFunc)(const String &niceName , DynamicObject * d);

  template <typename T>
  static CLASSNAME* createFromObject( const String &niceName , DynamicObject * d) {
    T * res =  new T(niceName);
    if(d){
    res->configureFromObject(d);
    }

    return res;
  }



  typedef CreatorFunc Entry;

  static  HashMap< String, Entry > & getFactory(){
    static HashMap< String, Entry > factory;
    return factory;
  }

};




#define REGISTER_OBJ_TYPE(FACTORY,T)  const Identifier T::_objType = Identifier( "t_" #T);\
static const Identifier _type_##T = FactoryBase<FACTORY>::registerType<T>(T::_objType.toString());


#define REGISTER_OBJ_TYPE_TEMPLATED(FACTORY,T,TT) template<> const Identifier T<TT>::_objType = Identifier(#T "_" #TT); \
static const String type_##T_##TT = FactoryBase<FACTORY>::registerType< T<TT> >(T<TT>::_objType.toString());



