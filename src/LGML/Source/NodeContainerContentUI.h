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
#include "ParameterProxy.h"
#include "NodeContainer.h"

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
	TextButton addParamProxyBT;

	OwnedArray<ControllableUI> proxiesUI;

	const int proxyUIHeight = 40;
	const int listGap = 2;

	void addParamProxyUI(ParameterProxy *);
	void removeParamProxyUI(ParameterProxy *);

	ControllableUI * getUIForProxy(ParameterProxy *);


	void updateSize();

	void init()override;
	void resized() override;

	void buttonClicked(Button * b) override;

	virtual void paramProxyAdded(ParameterProxy *) override;
	virtual void paramProxyRemoved(ParameterProxy *) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeContainerContentUI)
};


#endif  // NODECONTAINERCONTENTUI_H_INCLUDED
