/*
  ==============================================================================

    ControllerEditor.h
    Created: 10 May 2016 9:31:40am
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLEREDITOR_H_INCLUDED
#define CONTROLLEREDITOR_H_INCLUDED

#include "ControllerUI.h" //keep
#include "ControlVariableUI.h"
#include "GenericControllableContainerEditor.h"

class ControllerEditor : public InspectorEditor,
						 public ButtonListener,
						 public Controller::ControllerListener
{
public:
  ControllerEditor(Controller * controller,bool generateEditorFromParameters = true);
	virtual ~ControllerEditor();

	Controller * controller;

	virtual void resized() override;
	virtual int getContentHeight() override;

	int getVariablesHeight();

	//VARIABLES
	Component variablesContainer;

	TextButton addVariableBT;
	OwnedArray<ControlVariableUI> variablesUI;

  ScopedPointer<GenericControllableContainerEditor> editor;

	void addVariableUI(ControlVariable *, bool doResize = true);
	void removeVariableUI(ControlVariable *, bool doResize = true);

	ControlVariableUI * getUIForVariable(ControlVariable *);

	void buttonClicked(Button * b) override;
	void variableAddedAsync(Controller *, ControlVariable *) override;
	void variableRemovedAsync(Controller *, ControlVariable *) override;
  Rectangle<int> area;
  bool hideVariableUIs;
};


#endif  // CONTROLLEREDITOR_H_INCLUDED
