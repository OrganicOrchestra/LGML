/*
  ==============================================================================

    ControlManager.cpp
    Created: 2 Mar 2016 8:34:39pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControlManager.h"

ControllerManager::ControllerManager() :
	ControllableContainer("Controller Manager")
{
	setCustomShortName("control");
}

ControllerManager::~ControllerManager()
{

}

Controller * ControllerManager::addController(ControllerFactory::ControllerType controllerType)
{
	Controller * c = factory.createController(controllerType);
	controllers.add(c);
	listeners.call(&Listener::controllerAdded, c);
	c->addControllableListener(this);
	c->setParentContainer(this);
	return c;
}

void ControllerManager::removeController(Controller * c)
{
	c->removeListener(this);
	listeners.call(&Listener::controllerRemoved, c);
	controllers.removeObject(c);
}

void ControllerManager::askForRemoveController(Controller * c)
{
	removeController(c);
}
