/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#pragma once

// compile time check that all subclass has implemented
#define DECLARE_OBJECT_BASE(T)     virtual int dummy_##T() = 0;
#define CHK_OBJECT_BASE(T)  int dummy_check_##T = dummy_##T();ignoreUnused(dummy_check_##T);
#define IMPL_OBJECT_BASE(T)    virtual int dummy_##T() {return 0;};

class FactoryClass{
    
};

class FactoryObject
{
public:

    virtual ~FactoryObject() {};
    virtual const Identifier & getFactoryTypeId() const =0;
    virtual const String& getFactoryTypeName() const = 0;
    virtual void configureFromObject (DynamicObject*) = 0;
    virtual DynamicObject* createObject() = 0;
    virtual const String & getFactoryInfo() const = 0;


    template<class OtherType>
    bool isType() const {return getFactoryTypeId() == OtherType::typeId(); };
    
};






#define DECLARE_OBJ_TYPE_DEFAULTNAME(T,DEFAULTNAME,INFO) static const Identifier _factoryType; \
static const Identifier & typeId() {return _factoryType;}\
const Identifier & getFactoryTypeId() const override{return typeId();}\
const String & getFactoryTypeName() const override {return _factoryType.toString();}\
const String & getFactoryInfo() const override{static String i(INFO);return i;}\
explicit T(StringRef name=DEFAULTNAME); \


#define DECLARE_OBJ_TYPE(T,INFO) DECLARE_OBJ_TYPE_DEFAULTNAME(T,#T,INFO)
// use that for static objects that dont need factories
#define IMPL_OBJ_TYPE(T)  const Identifier T::_factoryType = Identifier( "t_" #T);

