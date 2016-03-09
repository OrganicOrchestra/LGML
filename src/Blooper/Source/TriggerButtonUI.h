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
#include "TriggerUI.h"

class TriggerButtonUI    : public TriggerUI , public Timer
{
public:
    TriggerButtonUI(Trigger * t);
    ~TriggerButtonUI();

    void paint (Graphics&);
    void resized();
    void mouseDown (const MouseEvent& event)override;
    void triggerTriggered(Trigger * p) override ;
    void startBlink();
    void timerCallback();
    void setTriggerReference(Trigger * t);
    float intensity;
    
private:
    
    int blinkTime;
    int refreshPeriod;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TriggerButtonUI)
};


#endif  // TRIGGERBUTTONUI_H_INCLUDED
