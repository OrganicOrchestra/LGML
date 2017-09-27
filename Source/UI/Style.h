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

#ifndef STYLE_H_INCLUDED
#define STYLE_H_INCLUDED
#pragma once
// TODO : implement nicer colour handling (maybe skinnable)
// Warning don't use JUCE's internal color definitions as it will be empty at this stage of compilation
#include "../JuceHeaderUI.h"

namespace LGMLColors
{
enum ColorsIds
{
    audioColor = 0x200001,
    dataColor,
    elementBackground
};

}


class AddElementButton : public Button
{
public:
    AddElementButton();

    void paintButton (Graphics& g,
                      bool isMouseOverButton,
                      bool isButtonDown)override;
    // utility function to share common positioning
    void setFromParentBounds (const Rectangle<int>& area);


};


#endif  // STYLE_H_INCLUDED
