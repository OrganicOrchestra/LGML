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


#include "ControllerUI.h"
#include "ControllerEditor.h"
#include "../../UI/Style.h"
#include "../../Controllable/Parameter/UI/TriggerBlinkUI.h"
#include "../../Controllable/Parameter/UI/StringParameterUI.h"
#include "../../Controllable/Parameter/UI/ParameterUIFactory.h"
#include "../../UI/Outliner.h"


static DrawableButton* getArrowButton(const String& name){
    static bool inited(false);
    static DrawablePath arrow;
    static DrawablePath  downArrow;
    static DrawablePath hoverArrow;
    static DrawablePath hoverDownArrow;
    if(!inited){
        Path p;
        p.addTriangle(0, 0, 1, 0.5, 0, 1);
        Path dp;
        dp.addTriangle(0, 0, 1, 0, 0.5, 1);
        arrow.setPath(p);
        downArrow.setPath(dp);
        hoverArrow.setPath(p);
        hoverDownArrow .setPath(dp);
        Colour hoverColour = arrow.getFill().fill.colour.contrasting(.8f);
        hoverArrow.setFill(hoverColour);
        hoverDownArrow.setFill(hoverColour);
        inited = true;
    }

    DrawableButton* res = new DrawableButton (name,DrawableButton::ButtonStyle::ImageStretched);

    res->setImages(&arrow,
                   &hoverArrow,// over,
                   &downArrow,// down,
                   nullptr,// disabled,
                   &downArrow,// normalOn,
                   &hoverDownArrow,// overOn,
                   &downArrow,// downOn,
                   nullptr// disabledOn
                   );
    return res;

}
ControllerUI::ControllerUI (Controller* controller) :
    InspectableComponent (controller, "controller"),
    controller (controller)
{
    canInspectChildContainersBeyondRecursion = true;
    {
        MessageManagerLock ml;
        addMouseListener (this, true);
    }
    nameTF = new StringParameterUI (controller->nameParam);
    nameTF->setBackGroundIsTransparent (true);
    addAndMakeVisible (nameTF);

    enabledBT = ParameterUIFactory::createDefaultUI (controller->enabledParam);
    addAndMakeVisible (enabledBT);

    Image removeImage = ImageCache::getFromMemory (BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

    removeBT.setImages (false, true, true, removeImage,
                        0.7f, Colours::transparentBlack,
                        removeImage, 1.0f, Colours::transparentBlack,
                        removeImage, 1.0f, Colours::white.withAlpha (.7f),
                        0.5f);
    removeBT.addListener (this);
    addAndMakeVisible (removeBT);


    inActivityBlink = new TriggerBlinkUI (controller->inActivityTrigger);
    outActivityBlink = new TriggerBlinkUI (controller->outActivityTrigger);
    inActivityBlink->showLabel = false;
    addAndMakeVisible (inActivityBlink);
    outActivityBlink->showLabel = false;
    addAndMakeVisible (outActivityBlink);
    userParamsUI = new Outliner("usr_"+controller->shortName,&controller->userContainer,false);
    userParamsUI->showUserContainer = true;
    addAndMakeVisible(userParamsUI);

    showUserParams = getArrowButton("showParams");
    addAndMakeVisible(showUserParams);
    showUserParams->setTooltip("show this controller registered parameters");
    showUserParams->setToggleState(false, dontSendNotification);
    showUserParams->addListener(this);
    showUserParams->setClickingTogglesState(true);

}

ControllerUI::~ControllerUI()
{

}

void ControllerUI::paint (Graphics& g)
{
    g.setColour (findColour (LGMLColors::elementBackground));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 4);

}

const int headerHeight = 20;
const int usrParamHeight = 200;

void ControllerUI::resized()
{
    Rectangle<int> area = getLocalBounds();
    Rectangle<int> r = area.removeFromTop(headerHeight);
    r.removeFromRight (15);
    removeBT.setBounds (r.removeFromRight (20));
    r.removeFromRight (2);
    outActivityBlink->setBounds (r.removeFromRight (r.getHeight()/2).reduced (2));
    inActivityBlink->setBounds (r.removeFromRight (r.getHeight()/2).reduced (2));

    enabledBT->setBounds (r.removeFromLeft (r.getHeight()));
    showUserParams->setBounds(r.removeFromLeft (r.getHeight()).reduced (4));
    r.removeFromLeft (5);
    nameTF->setBounds (r);

    if(area.getHeight()){
        userParamsUI->setBounds(area);
    }

}

int ControllerUI::getHeight(){
    return   headerHeight + (showUserParams->getToggleState()?jmax(usrParamHeight,userParamsUI->treeView.getViewport()->getViewArea().getHeight()):0);
}

void ControllerUI::mouseDown (const MouseEvent&)
{
    selectThis();
}

Component * getViewportOrParent( Component * c){
    Component * ic = c;
    while(ic){
        if(auto v = dynamic_cast<Viewport*>(ic)){
            return v->getParentComponent();
        }
        ic = ic->getParentComponent();
    }
    return c->getParentComponent();

}

void ControllerUI::buttonClicked (Button* b)
{
    if (b == &removeBT)
    {
        controller->remove();
    }
    else if(b==showUserParams){
        if(auto p = getViewportOrParent(this))
            p->resized();
        
    }
}

bool ControllerUI::keyPressed (const KeyPress& key)
{
    if (!isSelected) return false;

    if (key.getKeyCode() == KeyPress::deleteKey || key.getKeyCode() == KeyPress::backspaceKey)
    {
        controller->parentContainer->removeChildControllableContainer (controller);
        return true;
    }

    return false;
}

InspectorEditor* ControllerUI::createEditor()
{
#warning add factory to handle custom Editor
    return new GenericParameterContainerEditor(controller);
}
