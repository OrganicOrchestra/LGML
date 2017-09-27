/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#include "Style.h"

AddElementButton::AddElementButton(): Button ("Add") {};

void AddElementButton::paintButton (Graphics& g,
                                    bool isMouseOverButton,
                                    bool isButtonDown)
{

    auto area = getLocalBounds();
    auto bgColor = findColour (TextButton::buttonColourId);
    g.setColour ((isButtonDown || isMouseOverButton) ? bgColor.brighter() : bgColor);
    const float stroke = 1;
    g.drawEllipse (area.toFloat().reduced (stroke / 2), stroke);
    g.setColour (findColour (TextButton::textColourOffId));
    const float hw = stroke;//area.getHeight()/18.0;
    const float offset = area.getWidth() / 4 ;

    const float corner = hw;

    g.fillRoundedRectangle (area.getX() + offset, area.getCentre().getY() - hw, area.getWidth() - 2 * offset, 2 * hw, corner);
    g.fillRoundedRectangle ( area.getCentre().getX() - hw, area.getY() + offset, 2 * hw, area.getHeight() - 2 * offset,  corner);

}


void AddElementButton::setFromParentBounds (const Rectangle<int>& area)
{
    auto fmb = area;
    const int fms = jmin (jmin (36, area.getWidth()), area.getHeight());
    setBounds (fmb.removeFromBottom (fms).removeFromRight (fms));
}
