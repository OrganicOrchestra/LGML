/*
  ==============================================================================

    VuMeter.h
    Created: 8 Mar 2016 6:28:57pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef VUMETER_H_INCLUDED
#define VUMETER_H_INCLUDED

#include "NodeBase.h"

//TODO, move to more common place for use in other components
class VuMeter : public Component, public NodeAudioProcessor::RMSListener {
public:

	enum Type { IN,OUT};

    float voldB;
	Type type;

    VuMeter(Type _type) : type(_type)
	{
        setSize(8, 20);
        voldB = 0.f;
    }

    ~VuMeter(){

    }

    void paint(Graphics &g)override {


        g.setColour(NORMAL_COLOR);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);
        if (voldB > 0)
        {
            g.setGradientFill(ColourGradient(Colours::lightgreen, 0.f, getHeight()*.5f, Colours::red, 0.f, getHeight()*0.1f, false));
            g.fillRoundedRectangle(getLocalBounds().removeFromBottom((int)(getHeight()*(voldB))).toFloat(), 2.f);
        }
    }


    void RMSChanged(float rmsIn,float rmsOut) override {

		float rms = (type == Type::IN) ? rmsIn : rmsOut;

        float newVoldB = jmap<float>(20.0f*log10(rms/0.74f),0.0f,6.0f,0.85f,1.0f);

        if(newVoldB >= 0 && std::abs(newVoldB-voldB)>0.02f){
			setVoldB(newVoldB);
           
        }
    };

	void setVoldB(float value)
	{
		if (voldB == value) return;
		voldB = value;
		repaint();
	}
};



#endif  // VUMETER_H_INCLUDED
