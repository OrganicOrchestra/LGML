/*
==============================================================================

TriggerBlinkUI.h
Created: 8 Mar 2016 3:45:53pm
Author:  bkupe

==============================================================================
*/

#ifndef TRIGGERBLINKUI_H_INCLUDED
#define TRIGGERBLINKUI_H_INCLUDED



//==============================================================================
/*
*/


#include "ParameterUI.h"

class Trigger;

class TriggerBlinkUI : public ParameterUI, public Timer
{
public:
    TriggerBlinkUI(Trigger * t);
    virtual ~TriggerBlinkUI();

  
    void paint(Graphics&)override;
    void mouseDown(const MouseEvent& event)override;
    void startBlink();
    void timerCallback()override;
    Trigger * getTrigger();

    void valueChanged(const var & )override;

    float intensity;

	bool animateIntensity;
	int blinkTime;
	Colour offColor;
	Colour onColor;
private:

    int refreshPeriod;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriggerBlinkUI)
};


#endif  // TRIGGERBLINKUI_H_INCLUDED
