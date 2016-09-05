/*
  ==============================================================================

    ControllerEditor.cpp
    Created: 10 May 2016 9:31:40am
    Author:  bkupe

  ==============================================================================
*/

#include "ControllerEditor.h"
#include "Style.h"

ControllerEditor::ControllerEditor(ControllerUI * _controllerUI) :
	CustomEditor(_controllerUI),
	controller(_controllerUI->controller),
	addVariableBT("Add Variable")
{
	variablesContainer.addAndMakeVisible(&addVariableBT);
	addVariableBT.addListener(this);
	controller->addControllerListener(this);
	addAndMakeVisible(variablesContainer);

	for (auto &v : controller->variables)
	{
		addVariableUI(v, false);
	}
}

ControllerEditor::~ControllerEditor()
{
	controller->removeControllerListener(this);
}



void ControllerEditor::addVariableUI(ControlVariable * v, bool doResize)
{
	ControlVariableUI * vui = new ControlVariableUI(v);
	variablesContainer.addAndMakeVisible(vui);
	variablesUI.add(vui);

	vui->setNameIsEditable(true);

	if (doResize) resized();
}

void ControllerEditor::removeVariableUI(ControlVariable * v, bool doResize)
{
	ControlVariableUI * vui = getUIForVariable(v);
	if (vui == nullptr) return;
	variablesContainer.removeChildComponent(vui);
	variablesUI.removeObject(vui);
	if (doResize) resized();
}

ControlVariableUI * ControllerEditor::getUIForVariable(ControlVariable * v)
{
	for (auto &vui : variablesUI)
	{
		if (vui->variable == v) return vui;
	}

	return nullptr;
}

void ControllerEditor::resized()
{
	int variableUIHeight = 20;
	int listGap = 2;

	variablesContainer.setBounds(getLocalBounds().withHeight(getContentHeight()));
	Rectangle<int> r = variablesContainer.getLocalBounds();
	addVariableBT.setBounds(r.removeFromTop(20));
	r.removeFromTop(10);
	for (auto &vui : variablesUI)
	{
		vui->setBounds(r.removeFromTop(variableUIHeight));
		r.removeFromTop(listGap);
	}
}

int ControllerEditor::getContentHeight()
{
	int variableUIHeight = 20;
	int listGap = 2;

	return 30 + variablesUI.size()*(variableUIHeight + listGap);
}

void ControllerEditor::buttonClicked(Button * b)
{
	if (b == &addVariableBT)
	{
		Parameter * p = new FloatParameter(controller->getUniqueVariableNameFor("var"), "Custom Variable", 0, 0, 10);
		controller->addVariable(p);
	}
}

void ControllerEditor::variableAdded(Controller *, ControlVariable * v)
{
	addVariableUI(v);
	inspectorEditorListeners.call(&InspectorEditorListener::contentSizeChanged, this);
}

void ControllerEditor::variableRemoved(Controller *, ControlVariable * v)
{
	removeVariableUI(v);
	inspectorEditorListeners.call(&InspectorEditorListener::contentSizeChanged, this);
}