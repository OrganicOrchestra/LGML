/*
  ==============================================================================

    LGMLComponentHelpers.h
    Created: 9 Aug 2017 6:54:35pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

// quickHelper
template<typename T>
T * getParentOfType(Component * c){
  auto * i = c;
  while(i){
    if(T* r = dynamic_cast<T*>(i)){
      return  r;
    }
    i = i->getParentComponent();
  }
  return nullptr;
}
