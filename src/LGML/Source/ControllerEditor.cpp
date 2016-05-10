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
	addAndMakeVisible(&controllerLabel);
	controllerLabel.setText(controller->niceName, NotificationType::dontSendNotification);
	controllerLabel.setColour(controllerLabel.textColourId, TEXTNAME_COLOR);
}

ControllerEditor::~ControllerEditor()
{
}

void ControllerEditor::resized()
{
	Rectangle<int> r = getLocalBounds();
	controllerLabel.setBounds(r.removeFromTop(20));
}
