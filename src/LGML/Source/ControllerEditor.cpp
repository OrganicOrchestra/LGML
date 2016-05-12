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
	controller(_controllerUI->controller)
{
}

ControllerEditor::~ControllerEditor()
{
}
