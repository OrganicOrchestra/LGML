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
#include "GenericControllableContainerEditor.h"

class ControllerEditor : public InspectorEditor,
						 public ButtonListener
{
public:
  ControllerEditor(Controller * controller,bool generateEditorFromParameters = true);
	virtual ~ControllerEditor();

	Controller * controller;

	virtual void resized() override;
	virtual int getContentHeight() override;

	//VARIABLES
	TextButton addParameterBT;
  ScopedPointer<GenericControllableContainerEditor> editor;
	void buttonClicked(Button * b) override;
  
};


#endif  // CONTROLLEREDITOR_H_INCLUDED
