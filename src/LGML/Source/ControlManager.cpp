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
  controllers.clear();
//	while (controllers.size())
//	{
//		controllers[0]->remove();
//	}
}

var ControllerManager::getJSONData()
{
	var data = ControllableContainer::getJSONData();
    var controllersData;

    for (auto &c: controllers)
    {
        controllersData.append(c->getJSONData());
    }

    data.getDynamicObject()->setProperty("controllers", controllersData);
    return data;
}

void ControllerManager::loadJSONDataInternal(var data)
{
    clear();

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


