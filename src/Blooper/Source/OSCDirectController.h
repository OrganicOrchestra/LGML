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

class OSCDirectController : public OSCController, public ControllableContainer::Listener
{
public:
	OSCDirectController();

	void processMessage(const OSCMessage &msg) override;

	ControllerUI * createUI() override;

	// Inherited via Listener
	virtual void controllableAdded(Controllable * c) override;
	virtual void controllableRemoved(Controllable * c) override;
	virtual void controllableFeedbackUpdate(Controllable * c) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCDirectController)

};


#endif  // OSCDIRECTCONTROLLER_H_INCLUDED
