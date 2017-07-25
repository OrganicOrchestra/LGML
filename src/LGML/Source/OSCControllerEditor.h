/*
  ==============================================================================

    OSCControllerEditor.h
    Created: 10 May 2016 10:22:04am
    Author:  bkupe

  ==============================================================================
*/

#ifndef OSCCONTROLLEREDITOR_H_INCLUDED
#define OSCCONTROLLEREDITOR_H_INCLUDED

class OSCControllerUI;
#include "ControllerEditor.h"

#include "ParameterUI.h"
#include "OSCController.h"

class OSCControllerEditor : public ControllerEditor, public OSCController::OSCControllerListener
{
public:
	OSCControllerEditor(Controller * controller);
	virtual ~OSCControllerEditor();

	OSCController * oscController;

	ScopedPointer<ParameterUI> localPortUI;
	ScopedPointer<ParameterUI> remoteHostUI;
	ScopedPointer<ParameterUI> remotePortUI;
  ScopedPointer<ParameterUI> logIncomingOSC,logOutOSC;
  ScopedPointer<ParameterUI> speedLimit;
  ScopedPointer<ParameterUI> blockFeedback;
  ScopedPointer<ParameterUI> sendAllTrigger;


	Component innerContainer;
  
	virtual void resized() override;

	virtual int getContentHeight() override;

	// Inherited via OSCControllerListener
	virtual void messageProcessed(const OSCMessage & msg, bool success) override;
};


#endif  // OSCCONTROLLEREDITOR_H_INCLUDED
