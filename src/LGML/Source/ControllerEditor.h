/*
  ==============================================================================

    ControllerEditor.h
    Created: 10 May 2016 9:31:40am
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLEREDITOR_H_INCLUDED
#define CONTROLLEREDITOR_H_INCLUDED

#include "CustomEditor.h"
#include "ControllerUI.h" //keep
#include "ControlVariableUI.h"

class ControllerEditor : public CustomEditor,
						 public ButtonListener,
						 public Controller::ControllerListener
{
public:
	ControllerEditor(ControllerUI * controllerUI);
	virtual ~ControllerEditor();

	Controller * controller;

	virtual void resized() override;
	virtual int getContentHeight() override;

	int getVariablesHeight();

	//VARIABLES
	Component variablesContainer;

	TextButton addVariableBT;
	OwnedArray<ControlVariableUI> variablesUI;

	void addVariableUI(ControlVariable *, bool doResize = true);
	void removeVariableUI(ControlVariable *, bool doResize = true);

	ControlVariableUI * getUIForVariable(ControlVariable *);

	void buttonClicked(Button * b) override;
	void variableAddedAsync(Controller *, ControlVariable *) override;
	void variableRemovedAsync(Controller *, ControlVariable *) override;
};


#endif  // CONTROLLEREDITOR_H_INCLUDED
