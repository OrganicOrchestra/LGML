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


#ifndef NODEFACTORY_H_INCLUDED
#define NODEFACTORY_H_INCLUDED
#pragma once
#include "JuceHeader.h"//keep
#include "../../Utils/FactoryBase.h"

class NodeBase;
class NodeFactory:public FactoryBase<NodeBase>{

};

#define REGISTER_NODE_TYPE(T)  REGISTER_OBJ_TYPE(NodeBase,T);

#define REGISTER_NODE_TYPE_TEMPLATED(T,TT) REGISTER_OBJ_TYPE_TEMPLATED(NodeBase,T,TT);

//class NodeBase;
//class ConnectableNode;
//
//class NodeFactory
//{
//public:
//  NodeFactory();
//  ~NodeFactory();
//
//  static NodeBase* createFromVarObject(var v ,const String & name);
//  static NodeBase * createFromType(const String & type,DynamicObject * v = nullptr);
//
//
//
//  template<typename T>
//  static const String  registerType (const String & ID){
//    jassert(!getFactory().contains(ID));
//    getFactory().set(ID, Entry(createFromVar<T>));
//    return ID;
//  }
//
//  /////////
//  //intern
//  static void  logAllTypes();
//
//private:
//
//
//  typedef NodeBase* (*CreatorFunc)(const String &niceName, DynamicObject* params);
//
//  template <typename T>
//  static NodeBase* createFromVar( const String &niceName, DynamicObject* params) {
//    T * res =  new T();
//    if (niceName.isNotEmpty()){
//      res->setNiceName(niceName);
//    }
//    //    if(params){
//    //      res->setStateFromVar(*params);
//    //    }
//    return res;
//  }
//
//
//
//  typedef CreatorFunc Entry;
//
//  static  HashMap< String, Entry > & getFactory(){
//    static HashMap< String, Entry > factory;
//    return factory;
//  }
//};

//
//#define REGISTER_NODE_TYPE(T)  const String T::_nodeType = "t_" #T;\
//static const String _type_##T = NodeFactory::registerType<T>(T::_nodeType);
//
//
//#define REGISTER_NODE_TYPE_TEMPLATED(T,TT) template<> const String T<TT>::_nodeType = #T "_" #TT; \
//static const String type_##T_##TT = NodeFactory::registerType< T<TT> >(T<TT>::_nodeType);


#endif  // NODEFACTORY_H_INCLUDED
