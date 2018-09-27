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

#ifndef LOOKANDFEELOO_H_INCLUDED
#define LOOKANDFEELOO_H_INCLUDED

#pragma warning( disable : 4505 )

#include "../JuceHeaderUI.h"//keep





namespace LookAndFeelHelpers
{



forcedinline static Colour createBaseColour (const Colour &buttonColour,
                                             bool hasKeyboardFocus,
                                             bool isMouseOverButton,
                                             bool isButtonDown) noexcept
{
    const float sat = hasKeyboardFocus ? 1.3f : 0.9f;
    const Colour baseColour (buttonColour.withMultipliedSaturation (sat));

    if (isButtonDown)      return baseColour.contrasting (0.2f);

    if (isMouseOverButton) return baseColour.contrasting (0.1f);

    return baseColour;
}



};


class LookAndFeelOO: public LookAndFeel_V4
{
public:

    LookAndFeelOO();
    ~LookAndFeelOO();

    void drawButtonBackground (Graphics&, Button&, const Colour& backgroundColour,
                               bool isMouseOverButton, bool isButtonDown) override;

    void drawComboBox (Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       ComboBox&) override;
    Font getComboBoxFont (ComboBox&) override;
    void positionComboBoxText (ComboBox&, Label&) override;
    Label* createSliderTextBox (Slider&) override;
    void drawTextEditorOutline (Graphics& g, int width, int height, TextEditor& textEditor)override;

private:
    class SliderLabelComp;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LookAndFeelOO)
};

#endif  // LOOKANDFEELOO_H_INCLUDED
