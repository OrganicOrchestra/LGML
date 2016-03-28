/*
  ==============================================================================

    NodeBaseContentUI.h
    Created: 8 Mar 2016 5:53:45pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODEBASECONTENTUI_H_INCLUDED
#define NODEBASECONTENTUI_H_INCLUDED

#include "JuceHeader.h"
#include "UIHelpers.h"
#include "NodeBaseUI.h"

class NodeBaseContentUI : public ContourComponent
{
public:
    NodeBaseContentUI();
    virtual ~NodeBaseContentUI();

    NodeBaseUI * nodeUI;
    NodeBase * node;

    virtual void setNodeAndNodeUI(NodeBase * node,NodeBaseUI * nodeUI);
    virtual void init();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeBaseContentUI)
};

#endif  // NODEBASECONTENTUI_H_INCLUDED
