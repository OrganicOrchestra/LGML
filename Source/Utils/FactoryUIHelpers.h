/*
 ==============================================================================

 FactoryUIHelpers.h
 Created: 13 Sep 2017 5:51:05pm
 Author:  Martin Hermant

 ==============================================================================
 */

#pragma once
#include "JuceHeader.h"

namespace FactoryUIHelpers{


  template<class Factory>
  PopupMenu * getFactoryTypesMenuFilter(int menuIdOffset,Array<String> arr)
  {
    PopupMenu * p = new PopupMenu();
    int i= 0;
    auto a = Factory::getRegisteredTypes();
    for(auto & k:a){
      if( ! (arr.contains(k))){
        p->addItem(i+menuIdOffset,Factory::typeToNiceName(k));

      }
      i++;
    }

    return p;
  }

  template<class Factory>
  PopupMenu * getFactoryTypesMenu(int menuIdOffset)
  {
    PopupMenu * p = new PopupMenu();
    int i= 0;
    auto a = Factory::getRegisteredTypes();
    for(auto & k:a){
      p->addItem(i+menuIdOffset,Factory::typeToNiceName(k));

      i++;
    }

    return p;
  }



  template<class Factory>
  const String  getTypeNameFromMenuIdx(int idx){
    int count = 0;
    auto a = Factory::getRegisteredTypes();
    for(auto & k:a){
      if (idx==count){return k;}
      count++;
    }
    jassertfalse;
    return String::empty;
  }


  template<class obj>
  obj * createFromMenuIdx(int idx){
    const String type = getTypeNameFromMenuIdx<FactoryBase<obj> >(idx);
    return (obj*)FactoryBase<obj>::createFromTypeID(type);
  }
  
};
