/*
  ==============================================================================

    ShapeShifterPanelTab.cpp
    Created: 3 May 2016 12:43:01pm
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifterPanelTab.h"
#include "Style.h"


ShapeShifterPanelTab::ShapeShifterPanelTab(ShapeShifterContent * _content) : content(_content)
{
	panelLabel.setInterceptsMouseClicks(false, false);

	panelLabel.setFont(12);
	panelLabel.setColour(panelLabel.textColourId, TEXT_COLOR);
	panelLabel.setText(content->getName(), NotificationType::dontSendNotification);

	addAndMakeVisible(&panelLabel);

	setSize(panelLabel.getFont().getStringWidth(panelLabel.getText()) + 20, 20);
}

ShapeShifterPanelTab::~ShapeShifterPanelTab()
{
}

void ShapeShifterPanelTab::paint(Graphics & g)
{
	g.setColour(BG_COLOR);
	g.fillRect(panelLabel.getLocalBounds());
}

void ShapeShifterPanelTab::resized()
{
	panelLabel.setBounds(getLocalBounds());
}
