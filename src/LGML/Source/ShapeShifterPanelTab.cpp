/*
  ==============================================================================

    ShapeShifterPanelTab.cpp
    Created: 3 May 2016 12:43:01pm
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifterPanelTab.h"
#include "Style.h"


ShapeShifterPanelTab::ShapeShifterPanelTab(ShapeShifterContent * _content) : content(_content), selected(false)
{
	panelLabel.setInterceptsMouseClicks(false, false);

	panelLabel.setFont(12);
	panelLabel.setColour(panelLabel.textColourId, TEXT_COLOR);
	panelLabel.setJustificationType(Justification::centred);
	panelLabel.setText(content->getName(), NotificationType::dontSendNotification);

	addAndMakeVisible(&panelLabel);

	setSize(getLabelWidth(), 20);
}

ShapeShifterPanelTab::~ShapeShifterPanelTab()
{

}

void ShapeShifterPanelTab::setSelected(bool value)
{
	selected = value;
	repaint();
}

void ShapeShifterPanelTab::paint(Graphics & g)
{
	g.setColour(selected?BG_COLOR:BG_COLOR.brighter(.15f));
	Rectangle<int> r = getLocalBounds();
	if (!selected) r.reduce(1,1);
	g.fillRect(r);
}

void ShapeShifterPanelTab::resized()
{
	panelLabel.setBounds(getLocalBounds());
}

int ShapeShifterPanelTab::getLabelWidth()
{
	return panelLabel.getFont().getStringWidth(panelLabel.getText()) + 20;
}
