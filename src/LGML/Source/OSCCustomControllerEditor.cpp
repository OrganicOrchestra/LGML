/*
  ==============================================================================

    OSCCustomControllerEditor.cpp
    Created: 10 May 2016 2:29:40pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCCustomControllerEditor.h"

OSCCustomControllerEditor::OSCCustomControllerEditor(OSCCustomControllerUI * controllerUI) :
	OSCControllerEditor(controllerUI),
	customController(controllerUI->customController),
	addVariableBT("Add Variable")
{
	addAndMakeVisible(&addVariableBT);
	addVariableBT.addListener(this);
	customController->addControllerListener(this);

	for (auto &v : customController->variables)
	{
		addVariableUI(v,false);
	}
}

OSCCustomControllerEditor::~OSCCustomControllerEditor()
{
	customController->removeControllerListener(this);
}

void OSCCustomControllerEditor::addVariableUI(ControlVariable * v, bool doResize)
{
	ControlVariableUI * vui = new ControlVariableUI(v);
	addAndMakeVisible(vui);
	variablesUI.add(vui);

	vui->setNameIsEditable(true);

	if(doResize) resized();
}

void OSCCustomControllerEditor::removeVariableUI(ControlVariable * v, bool doResize)
{
	ControlVariableUI * vui = getUIForVariable(v);
	if (vui == nullptr) return;
	removeChildComponent(vui);
	variablesUI.removeObject(vui);
	if(doResize) resized();
}

ControlVariableUI * OSCCustomControllerEditor::getUIForVariable(ControlVariable * v)
{
	for (auto &vui : variablesUI)
	{
		if (vui->variable == v) return vui;
	}

	return nullptr;
}

void OSCCustomControllerEditor::resizedInternalOSC(Rectangle<int> r)
{
	addVariableBT.setBounds(r.removeFromTop(20));
	r.removeFromTop(10);
	for (auto &vui : variablesUI)
	{
		vui->setBounds(r.removeFromTop(20));
		r.removeFromTop(2);
	}
}

void OSCCustomControllerEditor::buttonClicked(Button * b)
{
	if (b == &addVariableBT)
	{
		Parameter * p = new FloatParameter("New Variable", "Custom Variable", .5f, 0, 1);
		p->replaceSlashesInShortName = false;
		customController->addVariable(p);
	}
}

void OSCCustomControllerEditor::variableAdded(Controller *, ControlVariable * v)
{
	addVariableUI(v);
}

void OSCCustomControllerEditor::variableRemoved(Controller *, ControlVariable * v)
{
	removeVariableUI(v);
}
