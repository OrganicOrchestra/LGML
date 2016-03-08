/*
  ==============================================================================

    OSCDirectControllerContentUI.h
    Created: 8 Mar 2016 10:47:26pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef OSCDIRECTCONTROLLERCONTENTUI_H_INCLUDED
#define OSCDIRECTCONTROLLERCONTENTUI_H_INCLUDED

#include "ControllerContentUI.h"

class OSCDirectControllerContentUI : public ControllerContentUI
{
public:
	OSCDirectControllerContentUI();

	void init() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCDirectControllerContentUI)
};


#endif  // OSCDIRECTCONTROLLERCONTENTUI_H_INCLUDED
