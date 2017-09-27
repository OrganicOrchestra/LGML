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


#pragma once

#include "ParameterUI.h"

#include "../RangeParameter.h"



class RangeParameterUI : public ParameterUI, private Slider::Listener
{
public:
    RangeParameterUI (RangeParameter* p): ParameterUI (p), slider (Slider::SliderStyle::TwoValueHorizontal, Slider::NoTextBox)
    {
        slider.addListener (this);
        addAndMakeVisible (slider);
        slider.setRange (p->minimumValue, p->maximumValue);
        slider.setMinAndMaxValues (p->getRangeMin(), p->getRangeMax());
    }
    Slider slider;
    void valueChanged (const var& v) override
    {
        if (v.isArray() && v.size() > 1)
        {
            slider.setMinAndMaxValues (v[0], v[1]);
        }

    };

    void rangeChanged (Parameter* p)override
    {
        auto rangeP = (RangeParameter*)p;
        slider.setRange (rangeP->minimumValue, rangeP->maximumValue);
    }
    void resized()override
    {
        slider.setBounds (getLocalBounds());
    }

    void sliderValueChanged (Slider* ) override
    {
        if(auto rangeP = ((RangeParameter*)parameter.get()) )
            rangeP->setValue (slider.getMinValue(), slider.getMaxValue());
        else
            jassertfalse;
    }



};
