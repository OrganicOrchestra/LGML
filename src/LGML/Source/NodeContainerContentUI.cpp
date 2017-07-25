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
#include "FloatParameter.h"

NodeContainerContentUI::NodeContainerContentUI() :
	ConnectableNodeContentUI(),
	editContainerBT("Edit Container"),
	addUserParamBT("Add Param Proxy")
{
	addAndMakeVisible(&editContainerBT);
	editContainerBT.addListener(this);

	addAndMakeVisible(&addUserParamBT);
	addUserParamBT.addListener(this);

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

	addUserParamBT.setBounds(r.removeFromTop(20));
	r.removeFromTop(10);



}


void NodeContainerContentUI::init()
{
  NodeContainer * nc = dynamic_cast<NodeContainer*>(node.get());
  if(!nc){jassertfalse;return;}
  nodeContainer = nc;
	nodeContainer->addNodeContainerListener(this);


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
	} else if (b == &addUserParamBT)
	{
		nodeContainer->addNewUserParameter<FloatParameter>("usrParam","userParameter desc" );
	}
}





