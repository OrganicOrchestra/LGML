/*
  ==============================================================================

    VuMeter.h
    Created: 8 Mar 2016 6:28:57pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef VUMETER_H_INCLUDED
#define VUMETER_H_INCLUDED

#include "UIHelpers.h"
#include "NodeBase.h"

//TODO, move to more common place for use in other components
class VuMeter : public ContourComponent, public NodeBase::NodeAudioProcessor::Listener {

public:
	VuMeter() {
		setSize(8, 20);
	}

	void paint(Graphics &g)override {


		g.setColour(NORMAL_COLOR);
		g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);
		if (vol > 0)
		{
			g.setGradientFill(ColourGradient(Colours::red, 0, getHeight(), Colours::lightgreen, 0, getLocalBounds().getCentreY(), false));
			g.fillRoundedRectangle(getLocalBounds().removeFromBottom(getHeight()*(vol)).toFloat(), 2);
		}
	}
	float vol;
	void RMSChanged(float v) override {
		vol = v;
		repaint();
	};

};



#endif  // VUMETER_H_INCLUDED
