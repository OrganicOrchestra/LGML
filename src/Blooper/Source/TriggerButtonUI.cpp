/*
  ==============================================================================

    TriggerButtonUI.cpp
    Created: 8 Mar 2016 3:45:53pm
    Author:  bkupe

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TriggerButtonUI.h"

//==============================================================================
TriggerButtonUI::TriggerButtonUI(Trigger *t):
trigger(t),
blinkTime(400),
refreshPeriod(10),
intensity(0)

{

    t->addListener(this);

}

TriggerButtonUI::~TriggerButtonUI()
{
}
void TriggerButtonUI::triggerTriggered(Trigger * p){
    
}
void TriggerButtonUI::paint (Graphics& g)
{

    g.setColour(Colours::white);
    g.drawRect(getLocalBounds());
    
    g.setColour(Colours::white.withAlpha(intensity));
    g.fillRect(getLocalBounds());
}


void TriggerButtonUI::startBlink(){
    intensity = 1;
    startTimer(refreshPeriod);
}
void TriggerButtonUI::timerCallback(){
    intensity-= refreshPeriod*1.0/blinkTime;
    if(intensity<0){
        intensity = 0;
        stopTimer();
    }
    repaint();
}
void TriggerButtonUI::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void TriggerButtonUI::mouseDown (const MouseEvent& event){
    trigger->trigger();
    startBlink();
}

