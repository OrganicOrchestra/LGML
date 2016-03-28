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
    DBG("direct controller ui constructor");
}

void OSCDirectControllerContentUI::init()
{
    DBG("init direct controller");
    oscd = (OSCDirectController *)controller;

    localPortUI = oscd->localPortParam->createStringParameterUI();
    remoteHostUI = oscd->remoteHostParam->createStringParameterUI();
    remotePortUI = oscd->remotePortParam->createStringParameterUI();

    addAndMakeVisible(localPortUI);
    addAndMakeVisible(remoteHostUI);
    addAndMakeVisible(remotePortUI);


    cui->setSize(300, 150);

    Array<Controllable *> nodeControllables = NodeManager::getInstance()->getAllControllables(true);
    for (auto &_c : nodeControllables)
    {
        DBG(_c->controlAddress+"     "+_c->description);
    }
}

void OSCDirectControllerContentUI::resized()
{
    Rectangle<int> r = getLocalBounds().reduced(10);
    localPortUI->setBounds(r.removeFromTop(localPortUI->getHeight()));
    r.removeFromTop(20);
    remoteHostUI->setBounds(r.removeFromTop(remoteHostUI->getHeight()));
    r.removeFromTop(5);
    remotePortUI->setBounds(r.removeFromTop(remotePortUI->getHeight()));

}

void OSCDirectControllerContentUI::mouseDown(const MouseEvent &)
{

    DBG("mouse down");

}
