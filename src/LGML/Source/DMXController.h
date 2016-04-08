/*
  ==============================================================================

    DMXController.h
    Created: 2 Mar 2016 8:51:24pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef DMXCONTROLLER_H_INCLUDED
#define DMXCONTROLLER_H_INCLUDED

#include "Controller.h"

class DMXController : public Controller
{
public:
    DMXController();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DMXController)
};


#endif  // DMXCONTROLLER_H_INCLUDED
