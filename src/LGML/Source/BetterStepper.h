/*
  ==============================================================================

    BetterIntStepper.h
    Created: 28 Sep 2016 3:14:10pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef BETTERINTSTEPPER_H_INCLUDED
#define BETTERINTSTEPPER_H_INCLUDED

#include "JuceHeader.h"

class BetterStepper : public Slider
{
public:
	BetterStepper(const String &tooltip);
	virtual ~BetterStepper();
  void resized()override;
  bool isMini;
	virtual void setTooltip(const String &tooltip) override;
//  void paint(Graphics &g) ;
};




#endif  // BETTERINTSTEPPER_H_INCLUDED
