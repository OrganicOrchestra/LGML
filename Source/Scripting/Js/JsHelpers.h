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


#ifndef JSHELPERS_H_INCLUDED
#define JSHELPERS_H_INCLUDED

#include "../../JuceHeaderCore.h"
namespace JsHelpers{
// static identifier allowing fast instanciation of js namespace identifiers
static const Identifier jsLocalIdentifier ("local");
static const Identifier jsGlobalIdentifier ("lgml");
static const Identifier jsPtrIdentifier ("_ptr");
static const Identifier jsGetIdentifier ("get");
static const Identifier jsTriggerIdentifier ("trigger");
static const Identifier jsSetIdentifier ("set");
static const Identifier jsArrayIdentifier ("elements");

static const Array<Identifier> jsCoreClasses =
{
    jsLocalIdentifier,
    jsGlobalIdentifier,
    Identifier ("Object"),
    Identifier ("Array"),
    Identifier ("String"),
    Identifier ("Math"),
    Identifier ("JSON"),
    Identifier ("Integer")
};



class JsObject{
public:
    virtual ~JsObject(){}
    virtual bool wasObjectDeleted()=0;
    virtual void* getVoid()=0;
};

template<class T>
class JsObjectRef : public JsObject,public WeakReference<T>{
public:
    JsObjectRef(T* ptr):WeakReference<T>(ptr){}
    bool wasObjectDeleted() override{return WeakReference<T>::wasObjectDeleted();}
    void* getVoid() override{return WeakReference<T>::get();}
};


class JsPtrStore{
public:
    juce_DeclareSingleton(JsPtrStore, true);


    typedef JsObject* SP;
    typedef int64 KeyType ;
    typedef HashMap<KeyType, SP> MapType;

    template<class T>
     KeyType create(T* o,bool allowRecreation=false){
         jassert(o!=nullptr);
         KeyType k = (int64)(void*)o;
         if(map.contains(k)){
//             handle multipe inheritance
             if(!allowRecreation && dynamic_cast<T*>((T*)map[k]->getVoid())){
                 return k;
             }
             jassert(allowRecreation );
             deleteEntry(k,false);
         }
         map.set(k,new JsObjectRef<T>(o));
        return k;
    }
    void deleteEntry(KeyType k,bool checkExistence = true){
        jassert(map[k]!=nullptr);
        if(!checkExistence || map.contains(k)){
            SP old = map[k];
            map.remove(k);
            delete old;
        }
    }

    void clear(){
        MapType::Iterator i (map);
        staticHeapSize = 0;
        Array<KeyType> toRm;
        while (i.next())
        {
            if(i.getValue()->wasObjectDeleted()){toRm.add(i.getKey());}
            else{staticHeapSize++;}
        }
        for(auto & k:toRm){deleteEntry(k,false);}
//        DBG("js static heap" <<staticHeapSize );

    }


    JsObject* retrieve(KeyType k){
        jassert(map.contains(k));
        return map.contains(k)?map[k]:nullptr;

    }

    static JsPtrStore * i(){return JsPtrStore::getInstance();}

    int staticHeapSize = 0;
    MapType map;

};



inline JsObject* getObjectPtrFromObject ( DynamicObject* d)
{

    if (!d)return nullptr;
    int64 p = (int64)d->getProperty (jsPtrIdentifier);
    if(p==-1)return nullptr;
    return JsPtrStore::i()->retrieve(p);
}


inline JsObject* getObjectPtrFromJS (const var::NativeFunctionArgs& a)
{
    return getObjectPtrFromObject(a.thisObject.getDynamicObject());
}



template<class T,class OriginClass=T>
inline T* castPtrFromObject ( DynamicObject* d)
{
    if(JsObject* p = getObjectPtrFromObject(d)){
        if(auto *ref  = dynamic_cast<JsObjectRef<OriginClass> * > (p)){
            return dynamic_cast<T * >(ref->get());
        }
    }
    return nullptr;

}

template<class T,class OriginClass=T>
inline T* castPtrFromJS (const var::NativeFunctionArgs& a){
    return castPtrFromObject<T,OriginClass>(a.thisObject.getDynamicObject());
}


template<class T >
inline void assignPtrToObject(T * ref,DynamicObject * dob,bool allowRecreation=false){
    dob->setProperty (jsPtrIdentifier,JsPtrStore::i()->create(ref,allowRecreation));
}


static inline void clearRefsFromVar(var va,bool recursive=true){
    auto * dob = va.getDynamicObject();
    if (dob){
        if(auto entry = (JsPtrStore::KeyType)dob->getProperty(jsPtrIdentifier)){
            JsPtrStore::i()->deleteEntry(entry);
        }
    }
    if(recursive){
        if(dob){
            for(auto & v : dob->getProperties()){
                clearRefsFromVar(v.value,true);
            }
        }

        else if(auto* dd = va.getArray()){
            for(auto a:*dd){
                clearRefsFromVar(a,true);
            }
        }
    }
}
static inline void clearRefsFromObj(DynamicObject* dob,bool recursive=true){
    clearRefsFromVar(var(dob),recursive);
}


inline String getJsFunctionNameFromAddress (const String& n)
{
    StringArray arr;
    arr.addTokens (n, "/", "");
    arr.remove (0);
    return    arr.joinIntoString ("_");

}

inline String getJsFunctionNameFromStringArray (const StringArray& arr)
{
    return    arr.joinIntoString ("_");

}

inline StringArray splitFunctionName (const Identifier& i)
{
    StringArray res;
    res.addTokens (i.toString(), "_", "");
    return res;

}
inline void addToNamespace (const String& elemName, DynamicObject* target, DynamicObject* global)
{
    jassert (target != nullptr);
    jassert (elemName != "");
    int idx = elemName.indexOfChar ('.');

    bool lastElem = (idx == -1);

    if (!lastElem)
    {
        String elem = elemName.substring (0, idx);
        DynamicObject* dd = global->getProperty (elem).getDynamicObject();

        if (dd == nullptr)
        {
            dd = new DynamicObject();
            global->setProperty (elem, dd);
        }

        addToNamespace (elemName.substring (idx + 1, elemName.length()), target, dd);
    }
    else
    {

        global->setProperty (elemName, target);
        //        registerNativeObject(elemName, target);
    }


}

inline void removeNamespaceFromObject (const String& ns, DynamicObject* d)
{
    if (d == nullptr)
    {
        DBG ("js Ns removing failed : not found " + ns);
        return;
    }

    int idx = ns.indexOfChar ('.');
    bool lastElem = (idx == -1);

    if (!lastElem)
    {
        String elem = ns.substring (0, idx);
        return removeNamespaceFromObject (ns.substring (idx + 1, ns.length()), d->getProperty (elem).getDynamicObject());
    }
    else
    {
        d->removeProperty (ns);
    }
}



inline DynamicObject*   getNamespaceFromObject (const String& ns, DynamicObject* d )
{
    if (d == nullptr)return nullptr;

    int idx = ns.indexOfChar ('.');

    bool lastElem = (idx == -1);

    if (!lastElem)
    {
        String elem = ns.substring (0, idx);
        return getNamespaceFromObject (ns.substring (idx + 1, ns.length()), d->getProperty (elem).getDynamicObject());
    }
    else
    {
        if (ns != "")
        {
            return d->getProperty (ns).getDynamicObject();
        }
        else
        {
            return d;
        }
    }
}



inline String namespaceToString (const NamedValueSet & v, int indentlevel = 0, bool showValue = false, bool showptr = false)
{
    String res;
    res += " (";
    bool initedComma = false;

    for (int i = 0 ; i < v.size() ; i++)
    {
        var* vv = v.getVarPointerAt (i);


        if (!vv->isObject() && !vv->isArray())
        {
            Identifier name = v.getName (i);

            if (name != jsPtrIdentifier)
            {
                res += (initedComma ? ", " : "") + name.toString() + (showValue ? '(' + vv->toString() + ")" : "") ;
                initedComma = true;
            }
        }
    }

    res += + ")\n";

    for (int i = 0 ; i < v.size() ; i++)
    {
        var* vv = v.getVarPointerAt (i);
        Identifier name = v.getName (i);



        if (vv->isObject() || vv->isArray())
        {
            for (int  j = 0 ; j < indentlevel ; j ++ )
            {
                res += "*.";
            }

            DynamicObject* d = nullptr;

            if (vv->isArray() && vv->getArray()->size() > 0)
            {
                res += "(Array[" + String (vv->getArray()->size()) + "])";
                d = vv->getArray()->getFirst().getDynamicObject();
            }
            else
            {
                d = vv->getDynamicObject();
            }

            if (showptr)
            {
                res += "(" + String::toHexString ((int64)d) + ")";
            }

            if (d != nullptr )
            {

                res += name + ":";
                res += namespaceToString (d->getProperties(), indentlevel + 1, showValue, showptr);
                

            }
            // potential empty array
            else if (!vv->isArray())
            {
                res += "unknown : " + vv->toString();
                jassertfalse;
            }

        }



    }

    return res;

}

inline Identifier getNumericIdentifier (int n)
{

    static const Array<Identifier> preAllocatedIds
    {
        Identifier ("0"), Identifier ("1"), Identifier ("2"), Identifier ("3"), Identifier ("4"),
        Identifier ("5"), Identifier ("6"), Identifier ("7"), Identifier ("8"), Identifier ("9"),
        Identifier ("10"), Identifier ("11"), Identifier ("12"), Identifier ("13"), Identifier ("14"),
        Identifier ("15"), Identifier ("16"), Identifier ("17"), Identifier ("18"), Identifier ("19"),
        Identifier ("20"), Identifier ("21"), Identifier ("22"), Identifier ("23"), Identifier ("24"),
        Identifier ("25"), Identifier ("26"), Identifier ("27"), Identifier ("28"), Identifier ("29"),
        Identifier ("30"), Identifier ("31"), Identifier ("32")
    };

    if (n < preAllocatedIds.size())
    {
        return preAllocatedIds.getUnchecked (n);
    }
    else
    {
        return Identifier (String (n));
    }


}


}

#endif  // JSHELPERS_H_INCLUDED
