/*
  ==============================================================================

    OSCDirectControllerContentUI.cpp
    Created: 8 Mar 2016 10:47:26pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCDirectControllerContentUI.h"
#include "NodeManager.h"

OSCDirectControllerContentUI::OSCDirectControllerContentUI()
{
}

void OSCDirectControllerContentUI::init()
{
	DBG("init direct controller");
	oscd = (OSCDirectController *)controller;

	localPortUI = oscd->localPortParam->getUI();
	remotePortUI = oscd->remotePortParam->getUI();

	addAndMakeVisible(localPortUI);
	addAndMakeVisible(remotePortUI);

	
	cui->setSize(300, 100);

	Array<Controllable *> nodeControllables = NodeManager::getInstance()->getAllControllables(true);
	for (auto &c : nodeControllables)
	{
		DBG(c->niceName + " : " + c->getControlAddress());
	}
}

void OSCDirectControllerContentUI::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(10);
	localPortUI->setBounds(r.removeFromTop(localPortUI->getHeight()));
	r.removeFromTop(5);
	remotePortUI->setBounds(r.removeFromTop(remotePortUI->getHeight()));

}

void OSCDirectControllerContentUI::mouseDown(const MouseEvent & e)
{
	
	DBG("mouse down");
	
}
