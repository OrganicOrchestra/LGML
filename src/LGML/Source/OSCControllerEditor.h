/*
  ==============================================================================

    OSCControllerEditor.h
    Created: 10 May 2016 10:22:04am
    Author:  bkupe

  ==============================================================================
*/

#ifndef OSCCONTROLLEREDITOR_H_INCLUDED
#define OSCCONTROLLEREDITOR_H_INCLUDED

#include "OSCController.h"
#include "ControllerEditor.h"

#include "TriggerBlinkUI.h"
#include "StringParameterUI.h"

class OSCControllerEditor : public ControllerEditor, public OSCController::OSCControllerListener
{
public:
	OSCControllerEditor(ControllerUI * controllerUI);
	virtual ~OSCControllerEditor();

	OSCController * oscController;

	ScopedPointer<StringParameterUI> localPortUI;
	ScopedPointer<StringParameterUI> remoteHostUI;
	ScopedPointer<StringParameterUI> remotePortUI;

	Trigger activityTrigger;
    ScopedPointer<TriggerBlinkUI> activityTriggerUI;

    StringArray activityLines;
    Label activityLog;

	void resizedInternal(Rectangle<int> bounds) override;

	// Inherited via OSCControllerListener
	virtual void messageProcessed(const OSCMessage & msg, bool success) override;
};


#endif  // OSCCONTROLLEREDITOR_H_INCLUDED
