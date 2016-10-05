/*
  ==============================================================================

    ProgressWindow.cpp
    Created: 5 Oct 2016 11:37:24am
    Author:  bkupe

  ==============================================================================
*/

#include "ProgressWindow.h"
#include "Style.h"

ProgressWindow::ProgressWindow(const String &_title) :
	progressParam("Progress", "Progression", 0, 0, 1),
	titleLabel("title","")
{
	DBG("progressParam " << progressParam.floatValue());

	progressUI = progressParam.createSlider();
	addAndMakeVisible(progressUI);
	addAndMakeVisible(titleLabel);
	titleLabel.setColour(Label::ColourIds::textColourId, TEXTNAME_COLOR);
	titleLabel.setText(_title, dontSendNotification);
}

ProgressWindow::~ProgressWindow()
{
}

void ProgressWindow::paint(Graphics & g)
{
	g.fillAll(Colours::black.withAlpha(.5f));
	
	Rectangle<int> r = getLocalBounds().withSizeKeepingCentre(windowWidth, windowHeight);
	g.setColour(PANEL_COLOR);
	g.fillRoundedRectangle(r.toFloat(), 2);
	g.setColour(PANEL_COLOR.brighter(.2f));
	g.drawRoundedRectangle(r.toFloat(), 2, 1);
}

void ProgressWindow::resized()
{
	Rectangle<int> r = getLocalBounds().withSizeKeepingCentre(windowWidth, windowHeight).reduced(5);
	titleLabel.setBounds(r.removeFromTop(15));
	r.removeFromTop(20);
	progressUI->setBounds(r.removeFromTop(20));
}

void ProgressWindow::setProgress(float progress)
{
	progressParam.setValue(progress);
}
