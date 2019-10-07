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

#if !ENGINE_HEADLESS

#include "BoolToggleUI.h"
#include "../../../UI/Style.h"
#include "../BoolParameter.h"
#include "../UndoableHelper.h"

#include "CachedNameLabel.hpp"
//==============================================================================
BoolToggleUI::BoolToggleUI ( ParameterBase* parameter) :
    ParameterUI (parameter),
   labelComp (new CachedNameLabel(this))
{
    addAndMakeVisible(labelComp.get());
    setSize (10, 10);

}

BoolToggleUI::~BoolToggleUI()
{

}

void BoolToggleUI::paint (Graphics& g)
{
    ParameterUI::paint(g);
    // we are on component deletion
    if (shouldBailOut())return;

    Colour onColour =
//    parameter->isEditable ?
        findColour (TextButton::buttonOnColourId)
//        :Colours::green
    ;

    bool valCheck = ((BoolParameter*)parameter.get())->invertVisuals ? !parameter->boolValue() : parameter->boolValue();
    Colour c =  valCheck ? onColour  : findColour (TextButton::buttonColourId);



    if(!parameter->isEditable){
        const int ledHeight=10;
        g.setColour(c);
        auto tbound = getLocalBounds()
                                    .removeFromLeft(ledHeight)
                                    .withSizeKeepingCentre(ledHeight, ledHeight)
                                    .reduced(1).toFloat() ;


            g.setColour(c);
            g.fillEllipse(tbound);


    }
    else{
        g.setGradientFill (ColourGradient (c.brighter(), (float)getLocalBounds().getCentreX(), (float)getLocalBounds().getCentreY(), c.darker(), 2.f, 2.f, true));
        g.fillRoundedRectangle (getLocalBounds().toFloat(), 2);
    }


    if(showLabel != labelComp->isVisible()){
        labelComp->setVisible(showLabel);
    }
//    g.setFont (fontHeight);
//    g.setColour (findColour(valCheck?TextButton::textColourOnId : TextButton::textColourOffId));

//    if (customTextDisplayed.isNotEmpty())
//    {
//        g.drawText (customTextDisplayed, getLocalBounds().reduced (2).toFloat(), Justification::centred);
//    }
//    else
//    {
//        g.drawText (juce::translate(parameter->niceName), getLocalBounds().reduced (2).toFloat(), Justification::centred);
//    }
//    }
}

void BoolToggleUI::mouseDown (const MouseEvent& e)
{
    ParameterUI::mouseDown (e);
    if (!parameter){jassertfalse; return;}
    if (!parameter->isEditable) return;

    if (e.mods.isLeftButtonDown() && !e.mods.isAnyModifierKeyDown())
    {
        UndoableHelpers::setValueUndoable(parameter, !parameter->boolValue());
        
    }
}
void BoolToggleUI::resized(){
    labelComp->setBounds(getLocalBounds());
}
void BoolToggleUI::mouseUp (const MouseEvent& e)
{
    ParameterUI::mouseUp (e);

    if (!parameter->isEditable) return;

    if (e.mods.isLeftButtonDown() && e.mods.isShiftDown()) parameter->setValue (!parameter->boolValue());
}

void BoolToggleUI::valueChanged (const var& )
{
    repaint();
}

#endif
