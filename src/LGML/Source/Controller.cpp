/*
  ==============================================================================

    Controller.cpp
    Created: 2 Mar 2016 8:49:50pm
    Author:  bkupe

  ==============================================================================
*/

#include "Controller.h"
#include "ControllerUI.h"
#include "ControllerFactory.h"

Controller::Controller(const String &name) :
    ControllableContainer(name)
{
    nameParam = addStringParameter("Name", "Set the name of the controller.",name);
    nameParam->addParameterListener(this);
    enabledParam = addBoolParameter("Enabled","Set whether the controller is enabled or disabled", true);
    enabledParam->addParameterListener(this);

    controllerTypeEnum = 0;//init
}


Controller::~Controller()
{
    DBG("Remove Controller");
}

ControllerUI * Controller::createUI()
{
    return new ControllerUI(this);
}

var Controller::getJSONData()
{
    var data(new DynamicObject());
    data.getDynamicObject()->setProperty("controllerType", ControllerFactory::controllerToString(this));
    //data.getDynamicObject()->setProperty("nodeId", String(id)); //no id for now


    //TODO should be moved in ControllableContainer getJSON generic method, so every controllableContainer will have already save/load functions.
    var paramsData;

    Array<Controllable *> cont = ControllableContainer::getAllControllables(true, true);

    for (auto &c : cont) {
        Parameter * base = dynamic_cast<Parameter*>(c);
        if (base)
        {
            var pData(new DynamicObject());
            pData.getDynamicObject()->setProperty("controlAddress", base->getControlAddress(this));
            pData.getDynamicObject()->setProperty("value", base->toString());
            paramsData.append(pData);
        }
        else if (dynamic_cast<Trigger*>(c) != nullptr) {

        }
        else {
            // should never happen un less another Controllable type than parameter or trigger has been introduced
            jassertfalse;
        }
    }

    data.getDynamicObject()->setProperty("parameters", paramsData);

    return data;
}

void Controller::loadJSONData(var data)
{

    //TODO : Move parameters save/load in ControllableContainer, so we only need to call ControllableContainer::loadJSONData() to handle parameters of a class inheriting CContainer;
    Array<var> * paramsData = data.getProperty("parameters", var()).getArray();

    for (var &pData : *paramsData)
    {
        String pControlAddress = pData.getProperty("controlAddress", var());// getProperymakeAddressFromXMLAttribute(paramXml->getAttributeName(i));

        Controllable * c = getControllableForAddress(pControlAddress, true, true);
        if (Parameter * p = dynamic_cast<Parameter*>(c)) {
            p->fromString(pData.getProperty("value", var())); //need to have a var-typed variable in parameter, so we can take advantage of autotyping
        }
        else {
            DBG("NodeBase::loadJSONData -> other Controllable than Parameters?");
            jassertfalse;
        }
    }
}

void Controller::remove()
{
    listeners.call(&Controller::Listener::askForRemoveController, this);
}

void Controller::parameterValueChanged(Parameter * p)
{
    if (p == nameParam) setNiceName(nameParam->value);
    else if (p == enabledParam)
    {
        DBG("set Controller Enabled " + String(enabledParam->value));
    }
}
