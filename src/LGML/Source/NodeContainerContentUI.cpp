/*
  ==============================================================================

    NodeContainerContentUI.cpp
    Created: 20 May 2016 12:27:56pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeContainerContentUI.h"
#include "NodeManagerUI.h"
#include "ShapeShifterManager.h"
#include "ControllableUI.h"

NodeContainerContentUI::NodeContainerContentUI() :
	ConnectableNodeContentUI(),
	editContainerBT("Edit Container"),
	addParamProxyBT("Add Param Proxy")
{
	addAndMakeVisible(&editContainerBT);
	editContainerBT.addListener(this);

	addAndMakeVisible(&addParamProxyBT);
	addParamProxyBT.addListener(this);

	setSize(250, 100);

}

NodeContainerContentUI::~NodeContainerContentUI()
{
	nodeContainer->removeNodeContainerListener(this);
}

void NodeContainerContentUI::resized()
{

	Rectangle<int> r = getLocalBounds().reduced(5);

	editContainerBT.setBounds(r.removeFromTop(20));
	r.removeFromTop(5);

	addParamProxyBT.setBounds(r.removeFromTop(20));
	r.removeFromTop(10);

	for (auto &pui : proxiesUI)
	{
		pui->setBounds(r.removeFromTop(proxyUIHeight));
		r.removeFromTop(listGap);
	}

}

void NodeContainerContentUI::updateSize()
{
	int targetHeight = 60 + proxiesUI.size()*(listGap + proxyUIHeight);
	setSize(getWidth(), targetHeight);
}

void NodeContainerContentUI::init()
{
  NodeContainer * nc = dynamic_cast<NodeContainer*>(node.get());
  if(!nc){jassertfalse;return;}
  nodeContainer = nc;
	nodeContainer->addNodeContainerListener(this);

	for (auto &p : nodeContainer->proxyParams)
	{
		addParamProxyUI(p);
	}

	updateSize();
}



void NodeContainerContentUI::buttonClicked(Button * b)
{
	if (b == &editContainerBT)
	{
		NodeManagerUI * nmui = ((NodeManagerUIViewport *)ShapeShifterManager::getInstance()->getContentForName(PanelName::NodeManagerPanel))->nmui;
		if (nmui != nullptr)
		{
			nmui->setCurrentViewedContainer(nodeContainer);
		}
	} else if (b == &addParamProxyBT)
	{
		nodeContainer->addParamProxy();
	}
}




void NodeContainerContentUI::addParamProxyUI(ParameterProxy * p)
{
	ControllableUI * cui = p->createDefaultUI();
	addAndMakeVisible(cui);
	proxiesUI.add(cui);
	updateSize();
}

void NodeContainerContentUI::removeParamProxyUI(ParameterProxy * p)
{
	ControllableUI * cui = getUIForProxy(p);
	if (cui == nullptr) return;
	removeChildComponent(cui);
	proxiesUI.removeObject(cui);
	updateSize();
}

ControllableUI * NodeContainerContentUI::getUIForProxy(ParameterProxy *p)
{
	for (auto &cui : proxiesUI)
	{
		if (cui->controllable == p) return cui;
	}

	return nullptr;
}



void NodeContainerContentUI::paramProxyAdded(ParameterProxy * p)
{
	addParamProxyUI(p);
}

void NodeContainerContentUI::paramProxyRemoved(ParameterProxy * p)
{
	removeParamProxyUI(p);
}
