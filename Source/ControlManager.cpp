/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
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



