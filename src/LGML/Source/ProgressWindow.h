/*
  ==============================================================================

    ProgressWindow.h
    Created: 5 Oct 2016 11:37:24am
    Author:  bkupe

  ==============================================================================
*/

#ifndef PROGRESSWINDOW_H_INCLUDED
#define PROGRESSWINDOW_H_INCLUDED

#include "JuceHeader.h"
#include "FloatParameter.h"
#include "FloatSliderUI.h"

class ProgressWindow : public Component
{
public:

	ProgressWindow(const String &title);
	~ProgressWindow();
	

	const int windowWidth = 300;
	const int windowHeight = 100;

	Label titleLabel;
	FloatParameter progressParam;
	ScopedPointer<FloatSliderUI> progressUI;
	
	void paint(Graphics & g) override;
	void resized() override;

	void setProgress(float progress);
};



#endif  // PROGRESSWINDOW_H_INCLUDED
