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


#ifndef FLOATSLIDERUI_H_INCLUDED
#define FLOATSLIDERUI_H_INCLUDED

#include "ParameterUI.h"

class FloatSliderUI    : public ParameterUI
{

public:
    FloatSliderUI(Parameter * parameter = nullptr);
    virtual ~FloatSliderUI();

    enum Direction { HORIZONTAL, VERTICAL };

    //settings
    Direction orientation;
	Colour defaultColor;

	bool changeParamOnMouseUpOnly;
    bool assignOnMousePosDirect;
    float scaleFactor;

    int fixedDecimals;

    //interaction
    float initValue;

    void paint(Graphics &g) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;

    float getValueFromMouse();
    float getValueFromPosition(const Point<int> &pos);

    virtual void setParamNormalizedValue(float value);
    virtual float getParamNormalizedValue();
    void rangeChanged(Parameter * )override{
      repaint();
    };


protected:
    void valueChanged(const var &) override ;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FloatSliderUI)
};


#endif  // FLOATSLIDERUI_H_INCLUDED
