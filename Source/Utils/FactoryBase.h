/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#pragma once
#include "../JuceHeaderCore.h"

static const Identifier factoryTypeIdentifier ("_tName");

template <typename CLASSNAME>
class FactoryBase
{
public:

    static CLASSNAME* createBaseFromObject (const String& name, DynamicObject* ob )
    {
        if (ob)
        {
            Identifier ID (ob->getProperty (factoryTypeIdentifier));
            return createFromTypeID (ID, name, ob);
        }

        jassertfalse;
        return nullptr;


    }

    static CLASSNAME* createFromTypeID (const Identifier& ID, const String& name = "", DynamicObject* ob = nullptr)
    {
        String className = ID.toString();
        String targetName = name.isEmpty() ? typeToNiceName (className) : name;

        if (getFactory().contains (className))
        {
            return getFactory()[className] (targetName, ob);
        }
        else
        {
            jassertfalse;
            return nullptr;
        }

    }
    static String typeToNiceName (const  String& t)
    {
        auto & snm = getShortNamesMap();
        if(snm.contains(t)){
            return snm.getReference(t);
        }

        jassertfalse; //
        if (t.length() > 2 && t[0] == 't' && t[1] == '_')
        {
            return t.substring (2);
        }

        jassertfalse;
        return t;

    }
    static String niceToTypeName (const  String& t)
    {

        auto it=getShortNamesMap().begin();
        auto end =getShortNamesMap().end();
        while(it!=end){
            if(it.getValue()==t)return it.getKey();
            it.next();

        }
        if (t.length() < 2 || (t[0] != 't' && t[1] != '_'))
        {
            return "t_" + t;
        }

        jassertfalse;
        return t;

    }

    static DynamicObject*   createTypedObjectFromInstance (CLASSNAME* c)
    {
        auto  res = c->getObject();
        res->setProperty (factoryTypeIdentifier, getFactoryTypeNameForInstance (c));
        return res;
    }

    static const Identifier& getFactoryTypeIdForInstance (CLASSNAME* i )
    {
        return i->getFactoryTypeId();
    };
    static const String& getFactoryTypeNameForInstance (CLASSNAME* i )
    {
        return i->getFactoryTypeId().toString();
    };

    static const String getFactoryNiceNameForInstance (CLASSNAME* i )
    {
        return typeToNiceName(i->getFactoryTypeId().toString());
    };


    template<typename T>
    static Identifier registerType (const String& ID,const String& shortName)
    {
        jassert (!getFactory().contains (ID));
        jassert (ID[0] == 't' && ID[1] == '_');
        getShortNamesMap().set(ID,shortName);
        // DBG("registering "+ID+"::"+shortName);
        getFactory().set (ID, Entry (createFromObject<T>));
        return Identifier(ID);
    }

    /////////
    //intern
    static void  logAllTypes()
    {
        DBG ("Factory types (" << typeid (CLASSNAME).name() << ") :");
        auto a = getRegisteredTypes();

        for (auto e : a)
        {
            DBG ("\t" + e);
        }
    }

    static Array<String> getRegisteredTypes()
    {
        Array<String> res;

        for (auto it = getFactory().begin(); it != getFactory().end() ; ++it)
        {
            res.add (it.getKey());
        }
        res.sort();

        return res;
    }

private:


    typedef CLASSNAME* (*CreatorFunc) (const String& niceName, DynamicObject* d);

    template <typename T>
    static CLASSNAME* createFromObject ( const String& niceName, DynamicObject* d)
    {
        T* res =  new T (niceName);

        if (d)
        {
            res->configureFromObject (d);
        }

        return res;
    }



    typedef CreatorFunc Entry;

    static  HashMap<String, String> & getShortNamesMap(){
        static HashMap<String, String> shortNamesMap; // readable class names (without suffixes)
        return shortNamesMap;
    }

    static  HashMap< String, Entry >& getFactory()
    {
        static HashMap< String, Entry > factory;
        return factory;
    }

};


#define REGISTER_OBJ_TYPE_NAMED(FACTORY,T,NAME,NICENAME) const Identifier T::_factoryType = FactoryBase<FACTORY>::registerType<T>(NAME,NICENAME);

#define REGISTER_OBJ_TYPE(FACTORY,T,NICENAME) REGISTER_OBJ_TYPE_NAMED(FACTORY,T,"t_" #T,NICENAME)


#define REGISTER_OBJ_TYPE_TEMPLATED(FACTORY,T,TT,NICENAME) template<> const Identifier T<TT>::_factoryType  = FactoryBase<FACTORY>::registerType< T<TT> >("t_" #T "_" #TT,NICENAME);



