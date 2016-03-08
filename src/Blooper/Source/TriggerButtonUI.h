/*
  ==============================================================================

    TriggerButtonUI.h
    Created: 8 Mar 2016 3:45:53pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef TRIGGERBUTTONUI_H_INCLUDED
#define TRIGGERBUTTONUI_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/

#include "Trigger.h"

class TriggerButtonUI    : public Component,public Trigger::Listener,Timer
{
public:
    TriggerButtonUI(Trigger * t);
    ~TriggerButtonUI();

    void paint (Graphics&);
    void resized();
    void mouseDown (const MouseEvent& event)override;
    void triggerTriggered(Trigger * p)override ;
    void startBlink();
    void timerCallback();
    void setTriggerReference(Trigger * t);
    float intensity;
    Trigger *  trigger;
private:
    
    int blinkTime;
    int refreshPeriod;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TriggerButtonUI)
};


#endif  // TRIGGERBUTTONUI_H_INCLUDED
