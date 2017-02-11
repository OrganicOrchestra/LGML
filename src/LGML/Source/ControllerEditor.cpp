/*
  ==============================================================================

    ControllerEditor.cpp
    Created: 10 May 2016 9:31:40am
    Author:  bkupe

  ==============================================================================
*/

#include "ControllerEditor.h"
#include "Style.h"

ControllerEditor::ControllerEditor(Controller * _controller,bool generateAuto) :
	InspectorEditor(),
	controller(_controller),
	addVariableBT("Add Variable"),
hideVariableUIs(false)
{
	variablesContainer.addAndMakeVisible(&addVariableBT);
	addVariableBT.addListener(this);
	controller->addControllerListener(this);
	addAndMakeVisible(variablesContainer);

	for (auto &v : controller->variables)
	{
		addVariableUI(v, false);
	}
  if(generateAuto){
  editor = new GenericControllableContainerEditor(controller);
  addAndMakeVisible(editor);
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
	InspectorEditor::resized();

	int variableUIHeight = 20;
	int listGap = 2;

  area =getLocalBounds();
  if(!hideVariableUIs){
	variablesContainer.setBounds(area.withHeight(getVariablesHeight()+variableUIHeight));
  Rectangle<int> r = variablesContainer.getBounds();
  area = area.withY(r.getBottom()+5);
	addVariableBT.setBounds(r.removeFromTop(variableUIHeight));

	for (auto &vui : variablesUI)
	{
		vui->setBounds(r.removeFromTop(variableUIHeight));
		r.removeFromTop(listGap);
	}

  }

  if(editor){
    editor->setBounds(area);
  }
}

int ControllerEditor::getContentHeight()
{
  return InspectorEditor::getContentHeight() + 5 + getVariablesHeight() + (editor?editor->getContentHeight():0);
}

int ControllerEditor::getVariablesHeight()
{
	int variableUIHeight = 20;
	int listGap = 2;
	return variablesUI.size()*(variableUIHeight + listGap);
}

void ControllerEditor::buttonClicked(Button * b)
{
	if (b == &addVariableBT)
	{
		Parameter * p = new FloatParameter(controller->getUniqueVariableNameFor("var"), "Custom Variable", 0, 0, 10);
		controller->addVariable(p);
	}
}

void ControllerEditor::variableAddedAsync(Controller *, ControlVariable * v)
{
	addVariableUI(v);
	inspectorEditorListeners.call(&InspectorEditorListener::contentSizeChanged, this);
}

void ControllerEditor::variableRemovedAsync(Controller *, ControlVariable * v)
{
	removeVariableUI(v);
	inspectorEditorListeners.call(&InspectorEditorListener::contentSizeChanged, this);
}


