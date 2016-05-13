/*
  ==============================================================================

    OSCCustomControllerEditor.cpp
    Created: 10 May 2016 2:29:40pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCCustomControllerEditor.h"
#include "DebugHelpers.h"

OSCCustomControllerEditor::OSCCustomControllerEditor(OSCCustomControllerUI * controllerUI) :
	OSCControllerEditor(controllerUI),
	customController(controllerUI->customController),
	addVariableBT("Add Variable")
{
	innerContainer.addAndMakeVisible(&addVariableBT);
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
	innerContainer.addAndMakeVisible(vui);
	variablesUI.add(vui);

	vui->setNameIsEditable(true);

	if(doResize) resized();
}

void OSCCustomControllerEditor::removeVariableUI(ControlVariable * v, bool doResize)
{
	NLOG("OSCCustomControllerEditor","removeVariableUI");
	ControlVariableUI * vui = getUIForVariable(v);
	if (vui == nullptr) return;
	innerContainer.removeChildComponent(vui);
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

void OSCCustomControllerEditor::resized()
{
	int variableUIHeight = 20;
	int listGap = 2;

	OSCControllerEditor::resized();
	Rectangle<int> r = innerContainer.getLocalBounds();
	addVariableBT.setBounds(r.removeFromTop(20));
	r.removeFromTop(10);
	for (auto &vui : variablesUI)
	{
		vui->setBounds(r.removeFromTop(variableUIHeight));
		r.removeFromTop(listGap);
	}
}

int OSCCustomControllerEditor::getContentHeight()
{
	int variableUIHeight = 20;
	int listGap = 2;

	int baseHeight = OSCControllerEditor::getContentHeight();
	return baseHeight + 30 + variablesUI.size()*(variableUIHeight + listGap);
}

void OSCCustomControllerEditor::buttonClicked(Button * b)
{
	if (b == &addVariableBT)
	{
		Parameter * p = new FloatParameter(customController->getUniqueVariableNameFor("var"), "Custom Variable", 0, 0, 1);
		p->replaceSlashesInShortName = false;
		customController->addVariable(p);
	}
}

void OSCCustomControllerEditor::variableAdded(Controller *, ControlVariable * v)
{
	addVariableUI(v);
	inspectorEditorListeners.call(&InspectorEditorListener::contentSizeChanged, this);
}

void OSCCustomControllerEditor::variableRemoved(Controller *, ControlVariable * v)
{
	removeVariableUI(v);
	inspectorEditorListeners.call(&InspectorEditorListener::contentSizeChanged, this);
}
