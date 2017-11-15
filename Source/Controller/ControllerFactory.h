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

#ifndef CONTROLLERFACTOR_H_INCLUDED
#define CONTROLLERFACTOR_H_INCLUDED

#include "../Utils/FactoryBase.h"
class Controller;
typedef FactoryBase<Controller> ControllerFactory;


#define REGISTER_CONTROLLER_TYPE(T)  REGISTER_OBJ_TYPE(Controller,T);

//#define REGISTER_CONTROLLER_TYPE_TEMPLATED(T,TT) REGISTER_OBJ_TYPE_TEMPLATED(NodeBase,T,TT);

#endif  // CONTROLLERFACTOR_H_INCLUDED
