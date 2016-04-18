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
    ControllableContainer("Controller Manager")
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

void ControllerManager::clear()
{
    while (controllers.size())
        controllers[0]->remove();
}

var ControllerManager::getJSONData()
{
    var data(new DynamicObject());
    var controllersData;

    for (auto &c: controllers)
    {
        controllersData.append(c->getJSONData());
    }

    data.getDynamicObject()->setProperty("controllers", controllersData);
    return data;
}

void ControllerManager::loadJSONData(var data, bool clearBeforeLoad)
{
    if (clearBeforeLoad) clear();

    Array<var> * controllersData = data.getProperty("controllers", var()).getArray();
    if (controllersData)
    {
        for (var &cData : *controllersData)
        {
            ControllerFactory::ControllerType controllerType = ControllerFactory::getTypeFromString(cData.getProperty("controllerType", var()));
            //int controllerId = cData.getProperty("controllerId", var());
            Controller* controller = addController(controllerType);
            controller->loadJSONData(cData);
        }
    }
    else
    {
        //DBG("No Controller to load");
    }


}

void ControllerManager::askForRemoveController(Controller * c)
{
    removeController(c);
}
