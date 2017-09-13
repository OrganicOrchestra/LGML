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


#include "NodeFactory.h"
//#include "../Impl/DummyNode.h"
//#include "../Impl/AudioMixerNode.h"
//#include "../../Data/DataInNode.h"
//#include "../Impl/Spat2DNode.h"
//#include "../Impl/VSTNode.h"
//#include "../Impl/AudioDeviceInNode.h"
//#include "../Impl/AudioDeviceOutNode.h"
//#include "../Impl/LooperNode.h"
//#include "../Impl/JsNode.h"

#include "../NodeContainer/NodeContainer.h"
//
//
//
//NodeFactory::NodeFactory()
//{
//}
//
//NodeFactory::~NodeFactory()
//{
//}
//
//
//NodeBase* NodeFactory::createFromVarObject(var v ,const String & name){
//  auto type =  v.getProperty("nodeType", var());
//  return createFromType(type,v.getDynamicObject());
//
//};
//
//NodeBase* NodeFactory::createFromType(const String & type,DynamicObject *  dobj){
//  if (getFactory().contains(type)){
//    return getFactory().getReference(type)(String::empty,dobj);
//  }
//  jassertfalse;
//  LOG("!!!Unknown Node type : " << type);
//  for(auto it = getFactory().begin();it != getFactory().end() ; ++it){
//    LOG("!!!"<< it.getKey());
//  }
//  return nullptr;
//};
//
//

//void  NodeFactory::logAllTypes(){
//  DBG("Node types");
//  for(auto it = getFactory().begin();it != getFactory().end() ; ++it){
//      DBG( "\t"<< it.getKey());
//  }
//
//}
//
//
