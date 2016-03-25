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
        voldB = 0;
	}
    ~VuMeter(){
        
    }

	void paint(Graphics &g)override {


		g.setColour(NORMAL_COLOR);
		g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);
		if (voldB > 0)
		{
			g.setGradientFill(ColourGradient(Colours::lightgreen, 0, getHeight()*.5, Colours::red, 0, getHeight()*0.1, false));
			g.fillRoundedRectangle(getLocalBounds().removeFromBottom(getHeight()*(voldB)).toFloat(), 2);
		}
	}
	float voldB;
	void RMSChanged(float rms) override {
        float newVoldB = jmap(20.0*log10(rms/0.74),0.0,6.0,0.85,1.0);
        if(newVoldB>=0 && std::abs(newVoldB-voldB)>0.02f){
            voldB = newVoldB;
            repaint();
        }
	};

};



#endif  // VUMETER_H_INCLUDED
