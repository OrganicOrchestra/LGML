/*
==============================================================================

TriggerBlinkUI.h
Created: 8 Mar 2016 3:45:53pm
Author:  bkupe

==============================================================================
*/

#ifndef TRIGGERBLINKUI_H_INCLUDED
#define TRIGGERBLINKUI_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/


#include "Trigger.h"
#include "TriggerUI.h"

class TriggerBlinkUI : public TriggerUI, public Timer
{
public:
	TriggerBlinkUI(Trigger * t);
	virtual ~TriggerBlinkUI();

	void paint(Graphics&);
	void resized();
	void mouseDown(const MouseEvent& event)override;
	void triggerTriggered(Trigger * p) override;
	void startBlink();
	void timerCallback();
	void setTriggerReference(Trigger * t);
	float intensity;

private:

	int blinkTime;
	int refreshPeriod;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriggerBlinkUI)
};


#endif  // TRIGGERBLINKUI_H_INCLUDED