/*
  ==============================================================================

    OSCDirectController.h
    Created: 8 Mar 2016 10:27:37pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef OSCDIRECTCONTROLLER_H_INCLUDED
#define OSCDIRECTCONTROLLER_H_INCLUDED

#include "OSCController.h"

class OSCDirectController : public OSCController
{
public:
	OSCDirectController();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCDirectController)
};


#endif  // OSCDIRECTCONTROLLER_H_INCLUDED
