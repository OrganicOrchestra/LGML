/*
  ==============================================================================

    BetterIntStepper.cpp
    Created: 28 Sep 2016 3:14:10pm
    Author:  bkupe

  ==============================================================================
*/

#include "BetterStepper.h"

BetterStepper::BetterStepper(const String &tooltip) : Slider(SliderStyle::IncDecButtons,TextEntryBoxPosition::TextBoxLeft)
{
	setTooltip(tooltip);
	setIncDecButtonsMode(IncDecButtonMode::incDecButtonsDraggable_AutoDirection);
  // TODO hack to disable it in sliding viewport only 
  setScrollWheelEnabled (false);
	
}

BetterStepper::~BetterStepper()
{
}


void BetterStepper::resized(){
  Slider::resized();
  isMini = getWidth() < 30;
  if(isMini){
    setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
  }
  else{
    setSliderStyle(SliderStyle::IncDecButtons);
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::white.withAlpha(0.1f));
    setColour(juce::Slider::textBoxTextColourId, juce::Colours::white.withAlpha(0.7f));
//    setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  }

}

//void BetterStepper::paint(juce::Graphics & g){
////  if(isMini){
////  g.drawText(String((int)Slider::getValue()), getLocalBounds(), juce::Justification::centred);
////  }
////  else{
//    Slider::paint(g);
////  }
//}
