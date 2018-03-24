/*
  ==============================================================================

    Point2DParameterUI.h
    Created: 23 Mar 2018 12:28:02pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once
#include "ParameterUI.h"
#include "../Point2DParameter.h"
#include "SliderUI.h"
template<class T>
class Point2DParameterUI : public ParameterUI{
public:
    Point2DParameterUI(Point2DParameter<T> *p):ParameterUI(p),sliderX(p->xParam),sliderY(p->yParam){
        addAndMakeVisible(sliderX);
        addAndMakeVisible(sliderY);
    }
    SliderUI<T> sliderX;
    SliderUI<T> sliderY;

    void resized()override{
        auto b = getLocalBounds();
        constexpr int gap = 2;
        sliderX.setBounds(b.removeFromLeft(b.getWidth()/2).reduced(gap,0));
        sliderY.setBounds(b.reduced(2,0));
    }
    void paint(Graphics& g) override{
//        auto b = getLocalBounds();
//        g.setColour(juce::Colours::red);
//        g.fillRect(b);

    }
    
};
