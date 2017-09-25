/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/



#include "TriggerBlinkUI.h"
#include "../../../UI/Style.h"
#include "../Trigger.h"
//==============================================================================
TriggerBlinkUI::TriggerBlinkUI(Trigger *t) :
	ParameterUI(t),
	blinkTime(200),
	refreshPeriod(40),
	intensity(0),
	animateIntensity(true),
	offColor(NORMAL_COLOR),
	onColor(HIGHLIGHT_COLOR)
{
    setSize(30, 20);

}

TriggerBlinkUI::~TriggerBlinkUI()
{

}


void TriggerBlinkUI::valueChanged(const var & ){
    startBlink();

}

void TriggerBlinkUI::paint(Graphics& g)
{
  if(parameter.get()){
	g.setColour(offColor.interpolatedWith(onColor,intensity));
    g.fillRoundedRectangle(getLocalBounds().toFloat(),2);
    g.setFont(10);
    g.setColour(Colours::white.darker(.1f));
	if (showLabel)
	{

      g.drawFittedText(customTextDisplayed.isNotEmpty()?customTextDisplayed:parameter->niceName, getLocalBounds().reduced(2), Justification::centred, 1);
	}
  }
}


void TriggerBlinkUI::startBlink(){
if(intensity<=0) startTimer(animateIntensity ? refreshPeriod : blinkTime);
  intensity = 1;
	if (!animateIntensity) repaint();



}

void TriggerBlinkUI::timerCallback(){

	if (animateIntensity)
	{
		intensity -= refreshPeriod*1.0f / blinkTime;

		if (intensity < 0) {
			intensity = 0;
			stopTimer();
		}
	} else
	{
		intensity = 0;
		stopTimer();
	}

    repaint();
}

Trigger* TriggerBlinkUI::getTrigger(){
  return dynamic_cast<Trigger*>(parameter.get());
}

void TriggerBlinkUI::mouseDown(const MouseEvent &e) {
	ParameterUI::mouseDown(e);
	if (e.mods.isLeftButtonDown())
	{
		getTrigger()->trigger();

	}
    
}
