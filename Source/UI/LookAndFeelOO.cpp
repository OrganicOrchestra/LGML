/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#if !ENGINE_HEADLESS
#include "LookAndFeelOO.h"
#include "Style.h"


// LEGACY Color
//const Colour NORMAL_COLOR    (0xff666666);
//const Colour BG_COLOR        (0xff222222);
//const Colour PANEL_COLOR     (0xff444444);
//const Colour FRONT_COLOR     (0xffCCCCCC);
//const Colour LIGHTCONTOUR_COLOR (0xffAAAAAA);
//const Colour HIGHLIGHT_COLOR (0xffFF922F);
//const Colour CONTOUR_COLOR   (0xcc303030);
//const Colour TEXT_COLOR      (0xffCCCCCC);
//const Colour TEXTNAME_COLOR  (0xff999999);
//const Colour AUDIO_COLOR     (0xff5f9ea0);//(Colours::cadetblue);//
//const Colour DATA_COLOR      (0xffffc0cb);//(Colours::pink);//
//const Colour FEEDBACK_COLOR       (0xff18b5ef);

//SLIDER 0xff99ff66


LookAndFeelOO::LookAndFeelOO()
{
    juce::LookAndFeel_V4::ColourScheme scheme (
        0xff222222,      //    windowBackground,
        0xff666666,      //    widgetBackground,
        0xff444444,      //    menuBackground,
        0xcc303030,      //    outline,
        Colours::white.darker(.1f),//0xffCCCCCC,      //    defaultText,
        Colour (0xff232323).brighter (0.5f), //    defaultFill,
        Colours::white.darker(.0f),      //    highlightedText,
        0xff111111,      //    highlightedFill,
        0xffCCCCCC       //    menuText,
    );
    LookAndFeel_V4::setColourScheme (scheme);
    setColour (Slider::trackColourId, Colour (0xff99ff66));
    setColour (Slider::backgroundColourId, Colours::black.withAlpha (0.1f));
    setColour (TextButton::buttonOnColourId, Colour (0xffFF922F));
//    setColour (TextButton::textColourOnId, Colours::black.brighter(.1f));
    setColour (Label::textWhenEditingColourId, Colours::white);

    setColour (TextEditor::ColourIds::highlightedTextColourId, Colours::white);
    setColour (TextEditor::ColourIds::highlightColourId, Colours::black);
    setColour(CaretComponent::caretColourId, Colours::white);
    setColour (TreeView::backgroundColourId,Colours::white.withAlpha(0.f)
               //scheme.getUIColour (ColourScheme::UIColour::windowBackground)
               );
    setColour(DrawableButton::backgroundOnColourId, Colours::transparentWhite);
    setColour(DrawableButton::backgroundColourId, Colours::transparentWhite);
    setColour (LGMLColors::audioColor, Colours::cadetblue);
    setColour (LGMLColors::dataColor, Colours::pink);
    setColour (LGMLColors::elementBackground, findColour (ResizableWindow::backgroundColourId).brighter (0.1f));
    setColour (TooltipWindow::ColourIds::textColourId, scheme.getUIColour (ColourScheme::UIColour::defaultText));



    setColour (0x1000440, /*LassoComponent::lassoFillColourId*/
               scheme.getUIColour (ColourScheme::UIColour::defaultFill).withAlpha(0.2f));
    setColour(0x1000441, /*LassoComponent::lassoOutlineColourId*/
              findColour(TextButton::buttonOnColourId));


}

LookAndFeelOO::~LookAndFeelOO()  {}

//==============================================================================

static void drawButtonShape (Graphics& g, const Path& outline, const Colour &baseColour, float height, bool isToggle)
{
    const float mainBrightness = baseColour.getBrightness();
    const float mainAlpha = baseColour.getFloatAlpha();

    auto bounds = outline.getBounds();

    if (isToggle)
    {
        g.setGradientFill (ColourGradient (baseColour.brighter(), bounds.getCentreX(), bounds.getCentreY(), baseColour.darker(), 2.f, 2.f, true));
    }
    else
    {
        g.setGradientFill (ColourGradient (baseColour.brighter (0.2f), 0.0f, 0.0f,
                                           baseColour.darker (0.25f), 0.0f, height, false));
    }

    //  g.setGradientFill(ColourGradient(baseColour.brighter(),(float)bounds.getCentreX(),(float)bounds.getCentreY(), baseColour.darker(), 2.f,2.f,true));
    //    g.setColour(baseColour);
    g.fillPath (outline);

    g.setColour (Colours::white.withAlpha (0.4f * mainAlpha * mainBrightness * mainBrightness));
    g.strokePath (outline, PathStrokeType (1.0f), AffineTransform::translation (0.0f, 1.0f)
                  .scaled (1.0f, (height - 1.6f) / height));

    g.setColour (Colours::black.withAlpha (0.4f * mainAlpha));
    g.strokePath (outline, PathStrokeType (1.0f));
}


void LookAndFeelOO::drawButtonBackground (Graphics& g,
                                          Button& button,
                                          const Colour& backgroundColour,
                                          bool isMouseOverButton,
                                          bool isButtonDown)
{
    Colour baseColour (backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                       .withMultipliedAlpha (button.isEnabled() ? 0.9f : 0.5f));

    if (isButtonDown || isMouseOverButton)
        baseColour = baseColour.contrasting (isButtonDown ? 0.2f : 0.1f);

    const bool flatOnLeft   = button.isConnectedOnLeft();
    const bool flatOnRight  = button.isConnectedOnRight();
    const bool flatOnTop    = button.isConnectedOnTop();
    const bool flatOnBottom = button.isConnectedOnBottom();

    const float width  = button.getWidth() - 1.0f;
    const float height = button.getHeight() - 1.0f;

    if (width > 0 && height > 0)
    {
        const float cornerSize = 3.0f;

        Path outline;
        outline.addRoundedRectangle (0.5f, 0.5f, width, height, cornerSize, cornerSize,
                                     ! (flatOnLeft  || flatOnTop),
                                     ! (flatOnRight || flatOnTop),
                                     ! (flatOnLeft  || flatOnBottom),
                                     ! (flatOnRight || flatOnBottom));

        drawButtonShape (g, outline, baseColour, height, button.getClickingTogglesState());

    }
}

constexpr int comboButtonW = 10;
void LookAndFeelOO::drawComboBox (Graphics& g, int width, int height, const bool isButtonDown,
                                  int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& box)
{
    buttonX = 2;
    buttonW = comboButtonW;
    g.fillAll (box.findColour (ComboBox::backgroundColourId));

    if (box.isEnabled() && box.hasKeyboardFocus (false))
    {
        g.setColour (box.findColour (ComboBox::buttonColourId));
        g.drawRoundedRectangle (0, 0, (float)width, (float)height, 2, 2);
    }
    else
    {
        g.setColour (box.findColour (ComboBox::outlineColourId));
        g.drawRoundedRectangle (0, 0, (float)width, (float)height, 2, 2);
    }

    //    const float outlineThickness = box.isEnabled() ? (isButtonDown ? 1.2f : 0.5f) : 0.3f;

//    const Colour baseColour (LookAndFeelHelpers::createBaseColour (box.findColour (ComboBox::buttonColourId),
//                                                                   box.hasKeyboardFocus (true),
//                                                                   false, isButtonDown)
//                             .withMultipliedAlpha (box.isEnabled() ? 1.0f : 0.5f));


    if (box.isEnabled())
    {
        const float arrowX = 1.f;
        const float arrowH = 0.3f;

        Path p;
        /*
        p.addTriangle (buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.45f - arrowH),
                       buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.45f,
                       buttonX + buttonW * arrowX,          buttonY + buttonH * 0.45f);
                       */


        p.addTriangle (buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.4f + arrowH),
                       buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.4f,
                       buttonX + buttonW * arrowX,          buttonY + buttonH * 0.4f);


        g.setColour (box.findColour (ComboBox::arrowColourId));
        g.fillPath (p);
    }
}

Font LookAndFeelOO::getComboBoxFont (ComboBox& box)
{
    return Font (jmin (15.0f, box.getHeight() * 0.75f));
}

void LookAndFeelOO::positionComboBoxText (ComboBox& box, Label& label)
{
    int buttonW = comboButtonW;
    label.setBounds (1 + buttonW, 1,
                     box.getWidth() - buttonW,
                     box.getHeight() - 2);

    label.setFont (getComboBoxFont (box));
}

class LookAndFeelOO::SliderLabelComp  : public Label
{
public:
    SliderLabelComp() : Label ("", "") {}

    void mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& d)
    {
        Component::mouseWheelMove (e, d);
    }
};


Label* LookAndFeelOO::createSliderTextBox (Slider& slider)
{
    Label* const l = new SliderLabelComp();

    l->setJustificationType (Justification::centred);
    l->setKeyboardType (TextInputTarget::decimalKeyboard);

    l->setColour (Label::textColourId, slider.findColour (Slider::textBoxTextColourId));
    l->setColour (Label::backgroundColourId,
                  (slider.getSliderStyle() == Slider::LinearBar || slider.getSliderStyle() == Slider::LinearBarVertical)
                  ? Colours::transparentBlack
                  : slider.findColour (Slider::textBoxBackgroundColourId));
    l->setColour (Label::outlineColourId, slider.findColour (Slider::textBoxOutlineColourId));
    
    return l;
}


void LookAndFeelOO::drawTextEditorOutline (Graphics& /*g*/, int /*width*/, int /*height*/, TextEditor& /*textEditor*/){
//    if (dynamic_cast<AlertWindow*> (textEditor.getParentComponent()) == nullptr)
//    {
//        if (textEditor.isEnabled())
//        {
//            if (textEditor.hasKeyboardFocus (true) && ! textEditor.isReadOnly())
//            {
//                g.setColour (textEditor.findColour (TextEditor::focusedOutlineColourId));
//            }
//            else
//            {
//                g.setColour (textEditor.findColour (TextEditor::outlineColourId));
//            }
//            g.drawRect (0, 0, width, height);
//        }
//    }
}

#endif
