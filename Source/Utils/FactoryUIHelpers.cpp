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

#if !ENGINE_HEADLESS

#include "FactoryUIHelpers.h"
#include "../Node/NodeBase.h"


namespace FactoryUIHelpers{
    // following nodes can't be added manually
static Array<String> nodeFilter  {"t_ContainerInNode", "t_ContainerOutNode","t_NodeManager"};

template<>
PopupMenu* getFactoryTypesMenu< NodeFactory > (int menuIdOffset ){
    return createFactoryTypesMenuFilter<NodeFactory >(nodeFilter,menuIdOffset);
}
}

#endif
