/*
  ==============================================================================

    ConnectableNodeContentUI.h
    Created: 8 Mar 2016 5:53:45pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef ConnectableNodeCONTENTUI_H_INCLUDED
#define ConnectableNodeCONTENTUI_H_INCLUDED

#include "JuceHeader.h"//keep

class ConnectableNodeUI;
class ConnectableNode;

class ConnectableNodeContentUI : public Component
{
public:
    ConnectableNodeContentUI();
    virtual ~ConnectableNodeContentUI();

    ConnectableNodeUI * nodeUI;
    WeakReference<ConnectableNode> node;

    virtual void setNodeAndNodeUI(ConnectableNode * node,ConnectableNodeUI * nodeUI);
    virtual void init();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectableNodeContentUI)
};

#endif  // ConnectableNodeCONTENTUI_H_INCLUDED
