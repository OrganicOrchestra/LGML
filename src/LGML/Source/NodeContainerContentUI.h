/*
  ==============================================================================

    NodeContainerContentUI.h
    Created: 20 May 2016 12:27:56pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODECONTAINERCONTENTUI_H_INCLUDED
#define NODECONTAINERCONTENTUI_H_INCLUDED

#include "ConnectableNodeContentUI.h"
#include "NodeContainer.h"
#include "ParameterUI.h"

class NodeContainerContentUI :
	public ConnectableNodeContentUI,
	public ButtonListener,
	public NodeContainerListener
{
public :
	NodeContainerContentUI();
	virtual ~NodeContainerContentUI();

	NodeContainer * nodeContainer;
	TextButton editContainerBT;
	TextButton addUserParamBT;

	OwnedArray<ParameterUI> userParamUI;



	void updateSize();

	void init()override;
	void resized() override;

	void buttonClicked(Button * b) override;

  
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeContainerContentUI)
};


#endif  // NODECONTAINERCONTENTUI_H_INCLUDED
