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
