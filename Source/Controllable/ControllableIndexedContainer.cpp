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

#include "ControllableIndexedContainer.h"

//Controllable * ControllableIndexedContainer::getControllableForAddress(StringArray addressSplit, bool /*recursive*/, bool /*getNotExposed*/)
//{
//
//    //Indexed container is not made to directly contain contrallable, but only multiple containers of the same type
//    bool isTargetAControllable = addressSplit.size() == 1;
//    if (isTargetAControllable) return nullptr;
//
//    int index = addressSplit[0].getIntValue();
//    if (index >= controllableContainers.size()) return nullptr;
//
//
//    addressSplit.remove(0);
//    return controllableContainers[index]->getControllableForAddress(addressSplit);
//}
