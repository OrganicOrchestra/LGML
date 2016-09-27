/*
  ==============================================================================

    OSCCustomControllerUI.h
    Created: 10 May 2016 2:29:34pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef OSCCUSTOMCONTROLLERUI_H_INCLUDED
#define OSCCUSTOMCONTROLLERUI_H_INCLUDED

#include "OSCControllerUI.h"
class OSCCustomController;
class OSCCustomControllerUI : public OSCControllerUI
{
public:
	OSCCustomControllerUI(OSCCustomController * controller);
	virtual ~OSCCustomControllerUI();

	OSCCustomController * customController;

	virtual InspectorEditor * getEditor() override;
};



#endif  // OSCCUSTOMCONTROLLERUI_H_INCLUDED
