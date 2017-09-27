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


#include "ParameterUI.h"
#include "../../../Utils/DebugHelpers.h"
#include "../../../UI/Style.h"
#include "../../../UI/LGMLDragger.h"
#include "../../../FastMapper/FastMapper.h"

//==============================================================================
ParameterUI::ParameterUI (Parameter* _parameter) :
    parameter (_parameter),
    showLabel (true),
    showValue (true),
    customTextDisplayed (String::empty),
    isMappingDest (false),
    isDraggable (true),
    isSelected (false)
{
    if (parameter.get())
    {
        parameter->addAsyncCoalescedListener (this);
        parameter->addParameterListener (this);
    }
    else
    {
        jassertfalse;
    }

    hasValidControllable = (parameter.get() != nullptr);
    jassert (hasValidControllable);
    setName (parameter->niceName);
    parameter->addControllableListener (this);
    mappingState = NOMAP;
    setMappingState (LGMLDragger::getInstance()->isMappingActive);
    updateTooltip();


}

ParameterUI::~ParameterUI()
{
    LGMLDragger::getInstance()->unRegisterDragCandidate (this);

    if (parameter.get())
    {
        parameter->removeControllableListener (this);
        parameter->removeParameterListener (this);
        parameter->removeAsyncParameterListener (this);
    }

    masterReference.clear();
}


void ParameterUI::setCustomText (const String text)
{
    customTextDisplayed = text;
    repaint();
}


void ParameterUI::mouseDown (const MouseEvent& e)
{
    if (e.mods.isRightButtonDown())
    {
        PopupMenu p;
        p.addItem (1, "Copy control address");
        p.addItem (2, "Add FastMap To");
        p.addItem (3, "Add FastMap From");
        int result = p.show();

        switch (result)
        {
            case 1:
                SystemClipboard::copyTextToClipboard (parameter->controlAddress);
                break;

            case 2:
                FastMapper::getInstance()->addFastMap()->referenceOut->setParamToReferTo (Parameter::fromControllable (parameter));
                break;

            case 3:
                FastMapper::getInstance()->addFastMap()->referenceIn->setParamToReferTo (Parameter::fromControllable (parameter));
                break;
        }
    }
}


bool ParameterUI::shouldBailOut()
{
    bool bailOut = parameter.get() == nullptr;

    // we want a clean deletion no?
    if (bailOut)
    {
        // TODO : changing vst preset sometimes hit that
        NLOG ("ParameterUI", "!!! old component still displayed : " << getName());
        //jassertfalse;
    }

    return bailOut;

}



void ParameterUI::controllableStateChanged (Controllable* c)
{
    setAlpha (c->enabled ? 1 : .5f);
}

void ParameterUI::controllableControlAddressChanged (Controllable*)
{
    updateTooltip();
    repaint();
}



void ParameterUI::updateTooltip()
{
    setTooltip (parameter->description + "\nControl Address : " + parameter->controlAddress);
}


class MapEffect : public ImageEffectFilter
{
public:
    MapEffect (const Colour& colour, uint32 _amount, String _text):
        amount (_amount),
        pRef (colour.getAlpha(), colour.getRed(), colour.getGreen(), colour.getBlue()),
        text (_text)
    {
    }
    PixelARGB pRef;
    uint32 amount;
    String text;
    uint32 trueAmount;

    template<typename T>
    void  applyFunction (Image::BitmapData& data)
    {
        for (int y = 0; y < data.height; ++y)
        {
            uint8* p = data.getLinePointer (y);

            for (int x = 0; x < data.width; ++x)
            {
                T* pp = ((T*) p);

                pp->desaturate();
                pp->tween (pRef, trueAmount);

                p += data.pixelStride;
            }
        }
    }
    void applyEffect (Image& image, Graphics& g, float /*scaleFactor*/, float alpha)
    {
        //  Image temp (image.getFormat(), image.getWidth(), image.getHeight(), true);
        trueAmount = (uint32) (alpha * alpha * amount);

        Image::BitmapData data (image, 0, 0, image.getWidth(), image.getHeight(), Image::BitmapData::readWrite);

        if (image.getFormat() == Image::PixelFormat::ARGB)
        {
            applyFunction<PixelARGB> (data);
        }
        else if (image.getFormat() == Image::PixelFormat::RGB)
        {
            applyFunction<PixelRGB> (data);
        }
        else
        {
            jassertfalse;
        }


        g.drawImage (image, image.getBounds().toFloat());
//        g.setColour (Colours::white);
//        g.drawFittedText (text, 0, 0, image.getWidth(), image.getHeight(), Justification::centred, 2);

    }
};



void  ParameterUI::setMappingState (const bool  b)
{
    if ( parameter && !parameter->isMappable())return;

    MappingState s = b ? (isMappingDest ? MAPDEST : MAPSOURCE) : NOMAP;

    if (s != mappingState)
    {
        if (s == NOMAP)
        {
            setInterceptsMouseClicks (true, true);

        }
        else
        {
            setInterceptsMouseClicks (true, false);
        }
    }

    if (b)
    {
        mapEffect  = new MapEffect (isMappingDest ? Colours::red : Colours::blue, 50, getName());
    }
    else
    {
        mapEffect = nullptr;
    }

    setComponentEffect (mapEffect);
    mappingState = s;
    repaint();
}




void ParameterUI::setMappingDest (bool _isMappingDest)
{
    isMappingDest = _isMappingDest;

    if (mappingState != NOMAP)
    {
        mappingState = isMappingDest ? MAPDEST : MAPSOURCE;
    }

}




//////////////////
// NamedParameterUI


NamedParameterUI::NamedParameterUI (ParameterUI* ui, int _labelWidth, bool labelA):
    ParameterUI (ui->parameter),
    ownedParameterUI (ui),
    labelWidth (_labelWidth),
    labelAbove (labelA)
{

    addAndMakeVisible (controllableLabel);

    controllableLabel.setJustificationType (Justification::centredLeft);
    controllableLabel.setText (ui->parameter->niceName, dontSendNotification);

    if (ui->parameter->isUserDefined)
    {
        controllableLabel.setEditable (true);
        controllableLabel.addListener (this);
    }

    controllableLabel.setTooltip (ParameterUI::getTooltip());

    addAndMakeVisible (ui);
    ui->toFront (false);
    setBounds (ownedParameterUI->getBounds()
               .withTrimmedRight (-labelWidth)
               .withHeight (jmax ((int)controllableLabel.getFont().getHeight() + 4, ownedParameterUI->getHeight())));
}

void NamedParameterUI::resized()
{
    Rectangle<int> area  = getLocalBounds();

    if (controllableLabel.getText().isNotEmpty())
    {
        if (labelAbove)
        {
            controllableLabel.setBounds (area.removeFromTop (jmin (18, area.getHeight() / 2)));
        }
        else
        {
            controllableLabel.setBounds (area.removeFromLeft (labelWidth));
            area.removeFromLeft (10);
        }
    }

    ownedParameterUI->setBounds (area);
}

void NamedParameterUI::labelTextChanged (Label* labelThatHasChanged)
{
    if (ownedParameterUI.get())
    {
        ownedParameterUI->parameter->setNiceName (labelThatHasChanged->getText());
    }
};

