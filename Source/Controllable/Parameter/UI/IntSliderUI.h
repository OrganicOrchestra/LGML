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


#ifndef INTSLIDERUI_H_INCLUDED
#define INTSLIDERUI_H_INCLUDED

#include "FloatSliderUI.h"

class IntSliderUI : public FloatSliderUI
{
public:
    IntSliderUI(Parameter * parameter);
    virtual ~IntSliderUI();

    void setParamNormalizedValue(float value) override;
    float getParamNormalizedValue() override;

protected:
    void valueChanged(const var&) override { repaint(); };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IntSliderUI)
};



#endif  // INTSLIDERUI_H_INCLUDED
