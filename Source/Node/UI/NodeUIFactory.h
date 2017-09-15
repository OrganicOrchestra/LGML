

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

#include "../../Utils/FactoryBase.h"

class ConnectableNodeUI;
class ConnectableNode;
class NodeUIFactory:public FactoryBase<ConnectableNodeUI>{
public:
  static ConnectableNodeUI * createDefaultUI(ConnectableNode * t);
};

#define REGISTER_NODEUI_TYPE(T) REGISTER_OBJ_TYPE(ConnectableNodeUI,T)


#define REGISTER_PARAM_TYPE_TEMPLATED(T,TT) REGISTER_PARAM_TYPE_TEMPLATED(NodeUI,T,TT)
