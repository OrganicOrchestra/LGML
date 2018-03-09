/*
  ==============================================================================

    FactoryUIHelpers.cpp
    Created: 7 Mar 2018 12:26:25pm
    Author:  Martin Hermant

  ==============================================================================
*/

#if !ENGINE_HEADLESS

#include "FactoryUIHelpers.h"
#include "../Node/NodeBase.h"


namespace FactoryUIHelpers{
    // following nodes can't be added manually
static Array<String> nodeFilter  {"t_ContainerInNode", "t_ContainerOutNode","t_NodeManager"};

template<>
PopupMenu* getFactoryTypesMenu< FactoryBase<NodeBase> > (int menuIdOffset ){
    return createFactoryTypesMenuFilter<FactoryBase<NodeBase> >(nodeFilter,menuIdOffset);
}
}

#endif
