/*
  ==============================================================================

    OSCController.h
    Created: 2 Mar 2016 8:50:08pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef OSCCONTROLLER_H_INCLUDED
#define OSCCONTROLLER_H_INCLUDED

#include "Controller.h"

class OSCController : public Controller
{
public:
	OSCController();
	virtual ~OSCController();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCController)
};



#endif  // OSCCONTROLLER_H_INCLUDED
