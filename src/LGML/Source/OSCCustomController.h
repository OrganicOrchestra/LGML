/*
  ==============================================================================

    OSCCustomController.h
    Created: 10 May 2016 2:29:25pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef OSCCUSTOMCONTROLLER_H_INCLUDED
#define OSCCUSTOMCONTROLLER_H_INCLUDED

#include "OSCDirectController.h"

class OSCCustomController : public OSCDirectController
{
public:
	OSCCustomController();
	virtual ~OSCCustomController();

	
	virtual var getJSONData() override;
	virtual void loadJSONData(var data) override;

	virtual ControllerUI * createUI() override;

	juce::Result processMessageInternal(const OSCMessage& msg) override;
};



#endif  // OSCCUSTOMCONTROLLER_H_INCLUDED
