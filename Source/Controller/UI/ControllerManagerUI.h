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


#ifndef CONTROLLERMANAGERUI_H_INCLUDED
#define CONTROLLERMANAGERUI_H_INCLUDED


#include "../ControllerManager.h"
#include "ControllerUI.h"

#include "../../UI/ShapeShifter/ShapeShifterContent.h"
#include "../../UI/Style.h"
//==============================================================================
/*
*/

class ControllerManagerUI : public juce::Component, ControllerManager::Listener
{
public:
    ControllerManagerUI (ControllerManager* manager);
    ~ControllerManagerUI();

    ControllerManager* manager;

    OwnedArray<ControllerUI> controllersUI;

    ControllerUI* addControllerUI (Controller* controller);
    void removeControllerUI (Controller* controller);

    ControllerUI* getUIForController (Controller* controller);

    void paint (Graphics&)override;
    void resized()override;

    void mouseDown (const MouseEvent& e) override;

    int getContentHeight() const;

    void clear();
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControllerManagerUI)

    // Inherited via Listener
    void controllerAdded (Controller*) override;
    void controllerRemoved (Controller*) override;
    void notifyParentViewPort() ;
};


class ControllerManagerUIViewport :
    public ShapeShifterContentComponent,
    private ButtonListener
{
public:
    ControllerManagerUIViewport (const String& contentName, ControllerManagerUI* _UI) :
        controllerManagerUI (_UI),
        ShapeShifterContentComponent (contentName)
    {
        vp.setViewedComponent (controllerManagerUI, true);
        vp.setScrollBarsShown (true, false);
        vp.setScrollOnDragEnabled (false);
        addAndMakeVisible (vp);
        vp.setScrollBarThickness (10);
        addAndMakeVisible (addControllerBt);
        addControllerBt.setTooltip ("Add controller");
        addControllerBt.addListener (this);


    }

    virtual ~ControllerManagerUIViewport()
    {

    }

    void resized() override
    {
        vp.setBounds (getLocalBounds());
        int th = jmax<int> (controllerManagerUI->getContentHeight(), getHeight());
        Rectangle<int> targetBounds = getLocalBounds().withPosition (controllerManagerUI->getPosition()).withHeight (th);

        targetBounds.removeFromRight (vp.getScrollBarThickness());
        bool needResize = controllerManagerUI->getHeight()==th;

        controllerManagerUI->setBounds (targetBounds);
        // resize not called if no change in heights, but internal state yes
        if(needResize){
            controllerManagerUI->resized();
        }

        if(controllerManagerUI->controllersUI.size()==0){
            int side = (int)( jmin(getWidth(),getHeight()) * .75);
            addControllerBt.setBounds(getLocalBounds().withSizeKeepingCentre(side,side));
        }
        else{
        addControllerBt.setFromParentBounds (getLocalBounds());
        }
    }

    void buttonClicked (Button*) override;

    Viewport vp;
    ControllerManagerUI* controllerManagerUI;
    AddElementButton addControllerBt;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControllerManagerUIViewport)
};
#endif  // CONTROLLERMANAGERUI_H_INCLUDED
