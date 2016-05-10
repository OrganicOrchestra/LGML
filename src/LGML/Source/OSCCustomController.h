/*
  ==============================================================================

    OSCCustomController.h
    Created: 10 May 2016 2:29:25pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef OSCCUSTOMCONTROLLER_H_INCLUDED
#define OSCCUSTOMCONTROLLER_H_INCLUDED

#include "OSCController.h"

class OSCCustomController : public OSCController
{
public:
	OSCCustomController();
	virtual ~OSCCustomController();

	
	virtual ControllerUI * createUI() override;

	Result processMessageInternal(const OSCMessage& msg) override;


};



#endif  // OSCCUSTOMCONTROLLER_H_INCLUDED
