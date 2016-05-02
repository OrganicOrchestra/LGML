/*
  ==============================================================================

    MovablePanel.cpp
    Created: 2 May 2016 3:08:37pm
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifterPanel.h"
#include "Style.h"

ShapeShifterPanel::ShapeShifterPanel(const String &_panelName, Component *_innerComponent) :
	header(_panelName),
	innerComponent(_innerComponent)	
{

	addAndMakeVisible(&header);
	addAndMakeVisible(innerComponent);
}

ShapeShifterPanel::~ShapeShifterPanel()
{
}


void ShapeShifterPanel::paint(Graphics & g)
{
	g.fillAll(BG_COLOR);
}

void ShapeShifterPanel::resized()
{
	Rectangle<int> r = getLocalBounds();
	header.setBounds(r.removeFromTop(headerHeight));
	innerComponent->setBounds(r);
}

ShapeShifterPanel::PanelHeader::PanelHeader(const String & _panelName)
{
	addAndMakeVisible(&panelLabel);
	panelLabel.setFont(12);
	panelLabel.setColour(panelLabel.textColourId, Colours::white);
	panelLabel.setText(_panelName, NotificationType::dontSendNotification);
}

ShapeShifterPanel::PanelHeader::~PanelHeader()
{
}

void ShapeShifterPanel::PanelHeader::paint(Graphics & g)
{
	g.setColour(BG_COLOR.brighter(.2f));
	g.fillRoundedRectangle(getLocalBounds().reduced(2).toFloat(),2);
}

void ShapeShifterPanel::PanelHeader::resized()
{
	panelLabel.setBounds(getLocalBounds());
}
