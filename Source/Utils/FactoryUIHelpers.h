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
#include "../JuceHeaderUI.h"
#include "FactoryBase.h"

class NodeBase;
extern UndoManager & getAppUndoManager();
namespace FactoryUIHelpers
{


template<class Factory>
PopupMenu* createFactoryTypesMenuFilter (Array<String> arr, int menuIdOffset = 1)
{
    jassert (menuIdOffset > 0);
    PopupMenu* p = new PopupMenu();
    int i = 0;
    auto a = Factory::getRegisteredTypes();

    for (auto& k : a)
    {
        if ( ! (arr.contains (k)))
        {
            p->addItem (i + menuIdOffset, Factory::typeToNiceName (k));

        }

        i++;
    }

    return p;
}

template<class Factory>
PopupMenu* getFactoryTypesMenu (int menuIdOffset = 1)
{
    jassert (menuIdOffset > 0);
    PopupMenu* p = new PopupMenu();
    int i = 0;
    auto a = Factory::getRegisteredTypes();
    
    for (auto& k : a)
    {
        p->addItem (i + menuIdOffset, Factory::typeToNiceName (k));

        i++;
    }

    return p;
}



template<class Factory>
const String  getFactoryTypeNameFromMenuIdx (int idx, int menuIdOffset = 1)
{
    int count = menuIdOffset;
    auto a = Factory::getRegisteredTypes();

    for (auto& k : a)
    {
        if (idx == count) {return k;}

        count++;
    }

    jassertfalse;
    return "";
}


template<class obj>
obj* createFromMenuIdx (int idx)
{
    const String type = getFactoryTypeNameFromMenuIdx<FactoryBase<obj> > (idx);
    return (obj*)FactoryBase<obj>::createFromTypeID (type);
}

template<class T>
class UndoableCreate:public UndoableAction{

    public :
    typedef std::function<T*()> addFType;
    typedef std::function<void(T*)> rmFType;
    UndoableCreate(addFType _addF,rmFType _rmF):addF(_addF),rmF(_rmF){

    }
    bool perform() override{
        obj=addF();
        return obj.get()!=nullptr;
    }

    bool undo() override{
        if(obj){
            rmF(obj);
            obj=nullptr;
            return true;
        }

        return false;
    }
    addFType addF;
    rmFType rmF;
    WeakReference<T> obj;
};

template<class T>
class UndoableFactoryCreateOrDelete:public UndoableAction{
    public:
        typedef std::function<void(T*)> addFType;
        typedef std::function<void(T*)> rmFType;
        UndoableFactoryCreateOrDelete(String _typeID,addFType _addF,rmFType _rmF,T * originToRemove,bool _isRemove):addF(_addF),rmF(_rmF),typeID(_typeID),obj(originToRemove),isRemove(_isRemove){
            obSettings = var(originToRemove?originToRemove->createObject():nullptr);
        };

        bool perform() override{
            return  isRemove?removeObj():createObj();
        }

        bool undo() override{
            return  !isRemove?removeObj():createObj();
        }

        bool removeObj(){
            jassert(obj);
            if(obj){
                rmF(obj);
                obj=nullptr;
                return true;
            }

            return false;
        }

    bool createObj(){
        if(!obj){
            obj = FactoryBase<T>::createFromTypeID(typeID,"",nullptr);
        }
        if(obj){
            addF(obj);
            obj->configureFromObject(obSettings.getDynamicObject());
            return true;
        }
        else{
            jassertfalse;
        }
        return false;
    }
    String typeID;
        addFType addF;
        rmFType rmF;
    WeakReference<T> obj;
    var obSettings;
    bool isRemove;

};


    //e xplicit template specialization to filter out uncreatable nodes
template<>
PopupMenu* getFactoryTypesMenu< FactoryBase<NodeBase> > (int menuIdOffset )
;

};


