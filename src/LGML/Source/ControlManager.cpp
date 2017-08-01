/*
 ==============================================================================

 ControlManager.cpp
 Created: 2 Mar 2016 8:34:39pm
 Author:  bkupe

 ==============================================================================
 */

#include "ControlManager.h"

juce_ImplementSingleton(ControllerManager);


ControllerManager::ControllerManager() :
ControllableContainer("Controllers")
{
  setCustomShortName("control");
}

ControllerManager::~ControllerManager()
{
  clear();
}


Controller * ControllerManager::addController(ControllerFactory::ControllerType controllerType)
{
  Controller * c = factory.createController(controllerType);
  c->nameParam->setValue(getUniqueNameInContainer(c->nameParam->stringValue()));

  controllers.add(c);

  addChildControllableContainer(c);
  listeners.call(&ControllerManager::Listener::controllerAdded, c);
  return c;
}

void ControllerManager::removeController(Controller * c)
{

  removeChildControllableContainer(c);
  listeners.call(&ControllerManager::Listener::controllerRemoved, c);
  controllers.removeObject(c);
}

void ControllerManager::clear()
{

  while (controllers.size())
  {
    controllers[0]->remove();
  }
}



ControllableContainer *  ControllerManager::addContainerFromVar(const String & name,const var & cData)
{


  ControllerFactory::ControllerType controllerType = ControllerFactory::getTypeFromString(cData.getProperty(Controller::controllerTypeIdentifier, var()));
  //int controllerId = cData.getProperty("controllerId", var());
  return addController(controllerType);

}



