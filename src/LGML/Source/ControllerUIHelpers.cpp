/*
  ==============================================================================

    ControllerUIHelpers.cpp
    Created: 13 May 2016 12:02:49pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControllerUIHelpers.h"
#include "ControlManager.h"
ControlVariableChooser::ControlVariableChooser()
{
	int controllerIndex = 1;
	for (auto &c : ControllerManager::getInstance()->controllers)
	{
		int targetStartIndex = controllerIndex * maxVariablesPerController; //max 1000 variables per controller, should be enough

		PopupMenu cp;
		int cIndex = targetStartIndex;
		for (auto &v : c->variables)
		{
			cp.addItem(cIndex, v->parameter->niceName);
			cIndex++;
		}

    addSubMenu(c->getNiceName(), cp);
		controllerIndex++;
	}
}

ControlVariableChooser::~ControlVariableChooser()
{
}

ControlVariable * ControlVariableChooser::showAndGetVariable()
{
	int result = show();

	if (result == 0) return nullptr;

	int controllerIndex = (int) floor(result / maxVariablesPerController) - 1;
	int variableIndex = result % maxVariablesPerController;

	return ControllerManager::getInstance()->controllers[controllerIndex]->variables[variableIndex];
}
