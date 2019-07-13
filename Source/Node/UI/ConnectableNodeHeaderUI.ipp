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

//#include "ConnectableNodeHeaderUI.h"
#include "ConnectableNodeUI.h"

#include "../../Preset/PresetChooserUI.h"
#include "../../Controllable/Parameter/UI/SliderUI.h"

#include "../../UI/VuMeter.h"
#include "../../Controllable/Parameter/UI/ParameterUIFactory.h"

ConnectableNodeHeaderUI::ConnectableNodeHeaderUI() :
    miniModeBT ("-"),
    bMiniMode (false)
{
    node = nullptr;
    nodeUI = nullptr;
    vuMeterIn = new VuMeter (VuMeter::Type::IN);
    vuMeterOut = new VuMeter (VuMeter::Type::OUT);

    setInterceptsMouseClicks(false,true);

    miniModeBT.addListener (this);
    LGMLUIUtils::optionallySetBufferedToImage(&miniModeBT);
    setMouseCursor(MouseCursor::ParentCursor);

    setSize (20, 30);
    setPaintingIsUnclipped(true);
    LGMLUIUtils::optionallySetBufferedToImage(this);
}

ConnectableNodeHeaderUI::~ConnectableNodeHeaderUI()
{
    if (node != nullptr)
    {
        node->removeRMSListener (vuMeterOut);
        node->removeRMSListener (vuMeterIn);

        node->removeControllableContainerListener (this);
        node->removeConnectableNodeListener (this);
    }
    else
    {
        jassertfalse;
    }

}

void ConnectableNodeHeaderUI::setNodeAndNodeUI (ConnectableNode* _node, ConnectableNodeUI* _nodeUI)
{
    node = _node;
    nodeUI = _nodeUI;


    node->addConnectableNodeListener (this);
    updateVuMeters();

    titleUI = new StringParameterUI (node->nameParam);
    titleUI->valueLabel.setEditable(false,true);
    titleUI->setBackGroundIsTransparent (true);
    LGMLUIUtils::optionallySetBufferedToImage(titleUI);
    addAndMakeVisible (titleUI);

    descriptionUI = new StringParameterUI (node->descriptionParam);
    descriptionUI->setBackGroundIsTransparent (true);
    descriptionUI->valueLabel.setEditable(false,true);
    LGMLUIUtils::optionallySetBufferedToImage(descriptionUI);
    addAndMakeVisible (descriptionUI);
    descriptionUI->valueLabel.setColour (Label::ColourIds::textColourId, findColour (Label::textColourId).darker (.3f));

    enabledUI = ParameterUIFactory::createDefaultUI (node->enabledParam);
    enabledUI->setCustomText("E");
    addAndMakeVisible (enabledUI);

    miniModeBT.setPaintingIsUnclipped(true);
    addAndMakeVisible (miniModeBT);


    if (node->canHavePresets())
    {
        presetChooser = new PresetChooserUI (node);
        addAndMakeVisible (presetChooser);
    }

    node->addControllableContainerListener (this);


    init();
    resized();

}

void ConnectableNodeHeaderUI::updateVuMeters()
{
    if (!vuMeterOut->isVisible() && node->hasAudioOutputs())
    {
        node->addRMSListener (vuMeterOut);
        addAndMakeVisible (vuMeterOut);

    }
    else if (vuMeterOut->isVisible() && !node->hasAudioOutputs())
    {
        node->removeRMSListener (vuMeterOut);
        vuMeterOut->setVisible (false);
    }

    if (!vuMeterIn->isVisible() && node->hasAudioInputs())
    {
        node->addRMSListener (vuMeterIn);
        addAndMakeVisible (vuMeterIn);
    }
    else if (vuMeterIn->isVisible() && !node->hasAudioInputs())
    {
        node->removeRMSListener (vuMeterIn);
        vuMeterIn->setVisible (false);
    }
}



void ConnectableNodeHeaderUI::init()
{
    //to overri
}

void ConnectableNodeHeaderUI::resized()
{
    if (!node) return;

    int vuMeterWidth = 8;
    int miniModeBTWidth = 15;




    Rectangle<int> r = getLocalBounds();


    r.reduce (4, 0);
    r.removeFromTop (4);

    if (node->hasAudioOutputs())
    {
        vuMeterOut->setBounds (r.removeFromRight (vuMeterWidth));
    }

    if (node->hasAudioInputs())
    {
        vuMeterIn->setBounds (r.removeFromLeft (vuMeterWidth));

    }

    r.reduce (5, 2);

    enabledUI->setBounds (r.removeFromLeft (10).reduced (0, 2));

    r.removeFromLeft (3);


    if (r.getWidth() < 100 && !bMiniMode)
    {
        miniModeBT.setVisible (false);
    }
    else
    {
        miniModeBT.setVisible (true);
        miniModeBT.setBounds (r.removeFromRight (miniModeBTWidth).reduced (0, 2));
        r.removeFromRight (2);
    }


    if (node->canHavePresets() && !bMiniMode )
    {
        int presetCBWidth = jmin (r.getWidth() / 3, 80);

        if (presetCBWidth < 20)
        {
            presetChooser->setVisible (false);
        }
        else
        {
            presetChooser->setVisible (true);
            presetChooser->setBounds (r.removeFromRight (presetCBWidth));
            r.removeFromRight (5);
        }

    }


    titleUI->setBounds (r.removeFromTop (12));
    descriptionUI->setBounds (r);
}

void ConnectableNodeHeaderUI::setMiniMode (bool value)
{
    if (bMiniMode == value) return;

    bMiniMode = value;

    if (bMiniMode)
    {
        if (node->canHavePresets()) removeChildComponent (presetChooser);

        miniModeBT.setButtonText ("+");
    }
    else
    {
        if (node->canHavePresets()) addChildComponent (presetChooser);

        miniModeBT.setButtonText ("-");
    }
}

void ConnectableNodeHeaderUI::nodeParameterChangedAsync (ConnectableNode*, ParameterBase* p)
{
    if (p == node->enabledParam)
    {
        if (!node->enabledParam->boolValue())
        {
            vuMeterOut->setVoldB (0);

        }


        Colour c = !node->enabledParam->boolValue() ? Colour (0xff3cacd5) : Colours::lightgreen;
        Colour c2 = !node->enabledParam->boolValue() ? Colours::blue.brighter (.6f) : Colours::red;
        vuMeterIn->colorLow = c;
        vuMeterOut->colorLow = c;
        vuMeterIn->colorHigh = c2;
        vuMeterOut->colorHigh = c2;

        postCommandMessage (repaintId);
    }

}


void ConnectableNodeHeaderUI::buttonClicked (Button* b)
{

    if (b == &miniModeBT)
    {
        nodeUI->setMiniMode(!nodeUI->isMiniMode);
    }
}


void ConnectableNodeHeaderUI::handleCommandMessage (int id)
{
    switch (id)
    {

        case repaintId:
            repaint();
            break;

        case audioInputChangedId:
        case audioOutputChangedId:
            updateVuMeters();
            resized();
            break;

        default:
            break;
    }
}



void ConnectableNodeHeaderUI::numAudioInputChanged (ConnectableNode*, int /*newNumInput*/)
{
    postCommandMessage (audioInputChangedId);

}
void ConnectableNodeHeaderUI::numAudioOutputChanged (ConnectableNode*, int /*newNumOutput*/)
{
    postCommandMessage (audioOutputChangedId);

}

String ConnectableNodeHeaderUI::getTooltip() {return nodeUI->getTooltip();};


#endif
