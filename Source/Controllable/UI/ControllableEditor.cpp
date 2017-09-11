/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include "ControllableEditor.h"
#include "ControllableUI.h"
#include "../Parameter/UI/ParameterUIFactory.h"

ControllableEditor::ControllableEditor(InspectableComponent * /*sourceComponent*/, Controllable * _controllable) :
	InspectorEditor(), controllable(_controllable),
	label("Label")
{
  ui = ParameterUIFactory::createDefaultUI(controllable->getParameter());
	addAndMakeVisible(ui);

	addAndMakeVisible(&label);

	label.setJustificationType(Justification::centred);
	label.setFont(label.getFont().withHeight(12));
	label.setText("Editing " + _controllable->niceName,dontSendNotification);
}

void ControllableEditor::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(2);
	label.setBounds(r.removeFromTop(12));

	r.removeFromTop(5);
	ui->setBounds(r.removeFromTop(20));
}

int ControllableEditor::getContentHeight()
{
	return 50;
}
