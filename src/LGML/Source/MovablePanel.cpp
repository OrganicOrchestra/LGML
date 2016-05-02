/*
  ==============================================================================

    MovablePanel.cpp
    Created: 2 May 2016 3:08:37pm
    Author:  bkupe

  ==============================================================================
*/

#include "MovablePanel.h"

MovablePanel::MovablePanel(const String &_panelName, Component *_innerComponent) :
	header(_panelName),
	innerComponent(_innerComponent)
{

	addAndMakeVisible(&header);
	addAndMakeVisible(innerComponent);
}

MovablePanel::~MovablePanel()
{
}

void MovablePanel::resized()
{
	Rectangle<int> r = getLocalBounds();
	header.setBounds(r.removeFromTop(headerHeight));
	innerComponent->setBounds(r);
}

MovablePanel::PanelHeader::PanelHeader(const String & _panelName)
{
	addAndMakeVisible(&panelLabel);
	panelLabel.setFont(12);
	panelLabel.setText(_panelName, NotificationType::dontSendNotification);
}

MovablePanel::PanelHeader::~PanelHeader()
{
}

void MovablePanel::PanelHeader::paint(Graphics & g)
{
}

void MovablePanel::PanelHeader::resized()
{
	panelLabel.setBounds(getLocalBounds());
}
